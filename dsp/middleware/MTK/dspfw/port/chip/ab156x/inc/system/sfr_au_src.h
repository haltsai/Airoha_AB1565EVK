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
#ifndef _AU_SRC_A_H_
#define _AU_SRC_A_H_
#include "types.h"
#include "sfr_au_dfe.h"

/******************************************************************************
 * Enumerations
 ******************************************************************************/
typedef enum AU_SRC_FS_IN_enum
{
    reserved_src,
    // SRC_IN_RATE_8K              = 0,
    // SRC_IN_RATE_11_025K         = 1,
    // SRC_IN_RATE_12K             = 2,
    // SRC_IN_RATE_16K             = 3,
    // SRC_IN_RATE_22_05K          = 4,
    // SRC_IN_RATE_24K             = 5,
    // SRC_IN_RATE_32K             = 6,
    // SRC_IN_RATE_44_1K           = 7,
    // SRC_IN_RATE_48K             = 8,
    // SRC_IN_RATE_64K             = 9,
    // SRC_IN_RATE_88_2K           = 10,
    // SRC_IN_RATE_96K             = 11,
    // SRC_IN_RATE_192K            = 12,
    SRC_IN_SW_DEF               = 13,

    // SRC_IN_MAX                  = 15,

    SRC_IN_RATE_8K              = 8000,
    SRC_IN_RATE_11_025K         = 11025,
    SRC_IN_RATE_12K             = 12000,
    SRC_IN_RATE_16K             = 16000,
    SRC_IN_RATE_22_05K          = 22050,
    SRC_IN_RATE_24K             = 24000,
    SRC_IN_RATE_32K             = 32000,
    SRC_IN_RATE_44_1K           = 44100,
    SRC_IN_RATE_48K             = 48000,
    SRC_IN_RATE_64K             = 64000,
    SRC_IN_RATE_88_2K           = 88200,
    SRC_IN_RATE_96K             = 96000,
    SRC_IN_RATE_192K            = 192000,

}AU_SRC_FS_IN;

typedef enum AU_SRC_FS_OUT_enum
{
    reserved1_src,
    // SRC_OUT_RATE_8K              = 0,
    // SRC_OUT_RATE_16K             = 1,
    // SRC_OUT_RATE_48K             = 2,
    // SRC_OUT_RATE_96K             = 3,
    // SRC_OUT_RATE_192K            = 4,
    SRC_OUT_SW_DEF               = 5,
    // SRC_OUT_SW_DEF               = 5,
    // SRC_OUT_SW_DEF_32K           = 6,
    // SRC_OUT_SW_DEF_44_1K         = 7,
    // SRC_OUT_SW_DEF_64K           = 8,
    // SRC_OUT_SW_DEF_88_2K         = 9,

    // SRC_OUT_MAX                  = 15,
    SRC_OUT_RATE_8K              = 8000,
    SRC_OUT_RATE_16K             = 16000,
    SRC_OUT_RATE_48K             = 48000,
    SRC_OUT_RATE_96K             = 96000,
    SRC_OUT_RATE_192K            = 192000,
    SRC_OUT_SW_DEF_32K           = 32000,
    SRC_OUT_SW_DEF_44_1K         = 44100,
    SRC_OUT_SW_DEF_64K           = 64000,
    SRC_OUT_SW_DEF_88_2K         = 88200,
}AU_SRC_FS_OUT;

typedef enum AU_SRC_RS_OUT_enum
{
    SRC_OUT_RS_DEF              = 0,
    SRC_OUT_RS_16BIT            = 2,//OUT_RS_16BIT,SRC input resolution 16bit change to byte
    SRC_OUT_RS_32BIT            = 4,//3,//OUT_RS_32BIT,SRC input resolution 32bit change to byte
}AU_SRC_SR_OUT;


typedef enum AU_SRC_DRIVING_MODE_enum
{
    reserved2_src,
     SRC_CDM                     = 0,/* CDM, Continuous Drive Mode*/
     SRC_IVDM                    = 1,/* IVDM, Input Vector Drive Mode*/
     SRC_OVDM                    = 2,/* OVDM, Output Vector Drive Mode*/
     SRC_CDMWM                   = 3,/* CDMWM, Continous Drive Mode + Write back to Memory*/
}AU_SRC_DRIVING_MODE;

typedef enum SRC_CHANNEL_s
{
    reserved3_src,
    // SRC_TWO_CH                  = 0,
    // SRC_CH0_ONLY                = 1,
    // SRC_CH1_ONLY                = 2,
    // SRC_TWO_CH_                 = 3,
}AU_SRC_CH;

typedef enum SRC_BIT_RES_e
{
    reserved4_src,
    // SRC_OUT_BIT_CSR_POSITION    = 8,
    // SRC_IN_BIT_CSR_POSITION     = 9,
} SRC_BIT_RES_t;

typedef enum AU_HW_SRC_SEL_s
{
    reserved5_src,
    // AU_HW_SRC_DISABLE          = 0,
    // AU_HW_SRC_A                = 1,
    // AU_HW_SRC_B                = 2,
    // AU_HW_SRC_C                = 3,
}AU_HW_SRC_SEL;

/******************************************************************************
 * SRC_SetA
 * Base Address = 0x62834000
 ******************************************************************************/

/* offset 0x00 */
typedef union SRC_CTL0_u
{
    // struct SRC_CTL0_s
    // {
        // U32 ENABLE              : 1;
        // U32 VECTOR_INTR_CLR     : 1;
        // U32 VECTOR_INTR_MASK    : 1;
        // U32 _RSVD_0_            : 13;
        // U32 VECTOR_MODE_LENGTH  : 16;
    // } field;
    // U32 reg;
} SRC_CTL0_t;

/* offset 0x04 */
typedef union SRC_CTL1_u
{
    // struct SRC_CTL1_s
    // {
        // U32 RESET         	    : 1;
        // U32 _RSVD_0_            : 31;
    // } field;
    // U32 reg;
} SRC_CTL1_t;


/* offset 0x08 */
typedef union SRC_SET_u
{
    // struct SRC_SET_s
    // {
        // AU_SRC_FS_IN        INPUT_SAMPLE_RATE   : 4;
        // AU_SRC_FS_OUT       OUTPUT_SAMPLE_RATE  : 4;
        // U32                 BIT_RESOLUTION      : 2;
        // AU_SRC_DRIVING_MODE DRIVING_MODE        : 2;
        // AU_SRC_CH           CHANNEL_SELECTION   : 2;
        // U32                 _RSVD_0_            : 2;
        // U32                 INPUT_INTR_SEL      : 3;
        // U32                 _RSVD_1_            : 1;
        // U32                 OUTPUT_INTR_SEL     : 3;
        // U32                 CORE_GATED_CLK      : 1;
        // U32                 VECTOR_INTR_SEL     : 3;
        // U32                 FOLLOW_SRCA         : 1;
        // U32                 SRC_GATED_CLK       : 3;
        // U32                 ROOT_CLK            : 1;
    // } field;
    // U32 reg;
} SRC_SET_t;

/* offset 0x0C */
typedef union SRC_CMP_u
{
    // struct SRC_CMP_s
    // {
        // U32 SRC_CMP_VAL         : 23;
        // U32 SRC_CMP_SGN         : 1;
        // U32 _RSVD_0_            : 7;
        // U32 UPDATE_SRC_CMP      : 1;
    // } field;
    // U32 reg;
} SRC_CMP_t;

/* offset 0x10 */
typedef union SRC_INTE_u
{
    // struct SRC_INTE_s
    // {
        // U32 INTEGER             : 11;
        // U32 _RSVD_0_            : 17;
        // U32 DEBUG_SEL           : 4;
    // } field;
    // U32 reg;
} SRC_INTE_t;

/* offset 0x14 */
typedef union SRC_FRAC_u
{
    // struct SRC_FRAC_s
    // {
        // U32 FRACTIONAL          : 25;
        // U32 _RSVD_0_            : 7;
    // } field;
    // U32 reg;
} SRC_FRAC_t;

typedef enum SRC_s_enum
{
    SRC_DEF                          = 0,
    ASRC_1                           = 1,//Enable HW ASRC_1
    SW_SRC                           = 2,//Enable SW SRC

} SRC_s, *SRC_PTR_s;

// EXTERN VOLATILE  SRC_s              SRC_A;               /* 0x42834000 */
// EXTERN VOLATILE  AU_ODFE_RADMA_s    AU_SRC_A_CH0_RADMA,  /* 0x42834100 */
                                    // AU_SRC_A_CH1_RADMA;  /* 0x42834200 */
// EXTERN VOLATILE  AU_IDFE_WADMA_s    AU_SRC_A_CH0_WADMA,  /* 0x42834300 */
                                    // AU_SRC_A_CH1_WADMA;  /* 0x42834400 */


// EXTERN VOLATILE  SRC_s              SRC_B;               /* 0x42838000 */
// EXTERN VOLATILE  AU_ODFE_RADMA_s    AU_SRC_B_CH0_RADMA,  /* 0x42838100 */
                                    // AU_SRC_B_CH1_RADMA;  /* 0x42838200 */
// EXTERN VOLATILE  AU_IDFE_WADMA_s    AU_SRC_B_CH0_WADMA,  /* 0x42838300 */
                                    // AU_SRC_B_CH1_WADMA;  /* 0x42838400 */

#endif
