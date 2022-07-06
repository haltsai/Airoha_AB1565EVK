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
 
#ifndef _BIT_H_
#define _BIT_H_

#include "types.h"



/**
 * Bit Mask
 */
#define BIT_MASK(n)         (1UL << (n))
#define BIT_MASK32(n)       ((U32)1 << (n))
#define BIT_MASK16(n)       ((U16)1 << (n))
#define BIT_MASK8(n)        ((U8)1 << (n))



/**
 * Bit Field
 */
#define BIT_FIELD(n,p)      ((n) << (p))

/**
 * Bit field operating
 */
#define BIT_FIELD_FILTER8(r,p,l)    (((U8)(r))  & ((((U8)1  << (l)) - 1) << (p)))
#define BIT_FIELD_FILTER16(r,p,l)   (((U16)(r)) & ((((U16)1 << (l)) - 1) << (p)))
#define BIT_FIELD_FILTER32(r,p,l)   (((U32)(r)) & ((((U32)1 << (l)) - 1) << (p)))
#define BIT_FIELD_CLEAR8(r,p,l)     (((U8)(r))  & ~((((U8)1  << (l)) - 1) << (p)))
#define BIT_FIELD_CLEAR16(r,p,l)    (((U16)(r)) & ~((((U16)1 << (l)) - 1) << (p)))
#define BIT_FIELD_CLEAR32(r,p,l)    (((U32)(r)) & ~((((U32)1 << (l)) - 1) << (p)))
#define BIT_FIELD_EXTRACT8(r,p,l)   (((U8)(r)  >> (p)) & (((U8)1  << (l)) - 1))
#define BIT_FIELD_EXTRACT16(r,p,l)  (((U16)(r) >> (p)) & (((U16)1 << (l)) - 1))
#define BIT_FIELD_EXTRACT32(r,p,l)  (((U32)(r) >> (p)) & (((U32)1 << (l)) - 1))
#define BIT_FIELD_INSERT8(r,p,l,v)  (BIT_FIELD_CLEAR8((r),(p),(l))  | ((U8)(v)  << (p)))
#define BIT_FIELD_INSERT16(r,p,l,v) (BIT_FIELD_CLEAR16((r),(p),(l)) | ((U16)(v) << (p)))
#define BIT_FIELD_INSERT32(r,p,l,v) (BIT_FIELD_CLEAR32((r),(p),(l)) | ((U32)(v) << (p)))

#define FIX_POINT_ROUNDING8(v,p)    (((U8)(v)  + ((U8)(v)  & ((U8)1  << (p-1)))) >> p)
#define FIX_POINT_ROUNDING16(v,p)   (((U16)(v) + ((U16)(v) & ((U16)1 << (p-1)))) >> p)
#define FIX_POINT_ROUNDING32(v,p)   (((U32)(v) + ((U32)(v) & ((U32)1 << (p-1)))) >> p)



#endif /* _BIT_H_ */

