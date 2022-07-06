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

#ifndef AUDIO_FFT_H
#define AUDIO_FFT_H

/*******************************************************************************
 * Include header files
 *******************************************************************************/
#include "DIF_FFT.h"
#include <stdint.h>
#include "types.h"
typedef enum {
   KAL_FALSE = 0,
   KAL_TRUE  = 1,
   kal_false = KAL_FALSE,
   kal_true  = KAL_TRUE,
} kal_bool;

/*==== CONSTANTS ==================================================*/

#define IMPORT  EXTERN
#ifndef __cplusplus
  #define EXTERN  extern
#else
  #define EXTERN  extern "C"
#endif
#define LOCAL     static
#define GLOBAL
#define EXPORT    GLOBAL


#define EQ        ==
#define NEQ       !=
#define AND       &&
#define OR        ||
#define XOR(A,B)  ((!(A) AND (B)) OR ((A) AND !(B)))

#ifndef FALSE
  #define FALSE   0
#endif

#ifndef TRUE
  #define TRUE    1
#endif

#ifndef NULL
  #define NULL    0
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

//enum {RX, TX, NONE};


#ifndef BOOL
typedef unsigned char  BOOL;
#endif

/*==== TYPES ======================================================*/

typedef unsigned char   kal_uint8;
//typedef signed char     S8;
typedef unsigned short  kal_uint16;
//typedef signed short    S16;
typedef unsigned int    kal_uint32;
//typedef signed int      S32;

typedef unsigned long long  kal_uint64;

typedef unsigned char   US8;
typedef unsigned short  US16;
typedef unsigned int    US32;
typedef unsigned long long  US64;

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long long   u64;

typedef volatile unsigned char  *P_kal_uint8;
typedef volatile signed char    *P_S8;
typedef volatile unsigned short *P_kal_uint16;
typedef volatile signed short   *P_S16;
typedef volatile unsigned int   *P_kal_uint32;
typedef volatile signed int     *P_S32;

typedef long            LONG;
typedef unsigned char   UBYTE;
typedef short           SHORT;

typedef signed char     kal_int8;
typedef signed short    kal_int16;
typedef signed int      kal_int32;
typedef long long       kal_int64;
typedef char            kal_char;

typedef unsigned int            *UINT32P;
typedef volatile unsigned short *UINT16P;
typedef volatile unsigned char  *UINT8P;
typedef unsigned char           *U8P;

typedef volatile unsigned char  *P_U8;
typedef volatile unsigned short *P_U16;
typedef volatile unsigned int   *P_U32;
typedef unsigned long long      *P_U64;
typedef signed long long        *P_S64;


//typedef unsigned char       U8;
//typedef unsigned short      U16;
//typedef unsigned int        U32;
typedef unsigned long long  U64;
typedef signed long long    S64;
#ifndef bool
typedef unsigned char       bool;
#endif



typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned int    UINT32;
typedef unsigned short  USHORT;
typedef signed char     INT8;
typedef signed short    INT16;
typedef signed int      INT32;
typedef signed int      DWORD;
typedef void            VOID;
typedef unsigned char   BYTE;
typedef float           FLOAT;

typedef char           *LPCSTR;
typedef short          *LPWSTR;

/*
 *	Types for stdio.h & stdlib.h & string.h
 */
typedef       long    off_t;
typedef unsigned int  size_t;
typedef off_t fpos_t;           /* stdio file position type */

#if 0
typedef long ptrdiff_t;
#endif

/*==== EXPORT =====================================================*/

//#define MAXIMUM(A,B)       (((A)>(B))?(A):(B))
//#define MINIMUM(A,B)       (((A)<(B))?(A):(B))


/*******************************************************************************
 * Compile Option
 *******************************************************************************/

/*******************************************************************************
 * Base Address Definition
 *******************************************************************************/

/*******************************************************************************
 * Public Function Declaration
 *******************************************************************************/
kal_bool ApplyFFT256(int16_t *pData, kal_uint16 u2DataStart, kal_uint32 *u4FreqData, kal_uint32 *u4MaxData, UINT32 dSR);
kal_bool FreqCheck(kal_uint32 u4TargetFreq, kal_uint32 u4FreqData);
kal_bool MagnitudeCheck(kal_uint32 u4LboundMag, kal_uint32 u4UboundMag, kal_uint32 *u4MagData);

#endif /*AUDIO_FFT_H*/

