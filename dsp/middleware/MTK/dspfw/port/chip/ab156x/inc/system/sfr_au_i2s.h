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
#ifndef _AU_I2S_SFR_H_
#define _AU_I2S_SFR_H_
#include "types.h"


/******************************************************************************
 * Enumerations
 ******************************************************************************/
typedef enum AU_I2S_MODE_enum
{
    reserved_i2s,
    // I2S_TX_MODE     = 0,
    // I2S_RX_MODE     = 1,
    // I2S_TRX_MODE    = 2,
    // I2S_DISABLE     = 3,

}AU_I2S_MODE;



/******************************************************************************
 * I2S
 * Base Address = 0x42840000
 ******************************************************************************/

 /* offset 0x00 */
typedef union I2S_CTL0_u
{
    struct I2S_CTL0_s
    {
        U32 EN_I2S0_MOD	        : 1;
        U32 _RSVD_0_         	: 1;
        U32 EN_I2S1_MOD	        : 1;
        U32 _RSVD_1_         	: 11;
        U32 DBG_PORT_SEL        : 2;
        U32 _RSVD_2_       		: 16;
    } field;
    U32 reg;
} I2S_CTL0_t;

 /* offset 0x04 */
typedef union I2S_CTL1_u
{
    struct I2S_CTL1_s
    {
        U32 RST_I2S0_MOD	    : 1;
        U32 _RSVD_0_         	: 1;
        U32 RST_I2S1_MOD	    : 1;
        U32 _RSVD_1_         	: 29;
    } field;
    U32 reg;
} I2S_CTL1_t;

 /* offset 0x08 */
typedef union I2S_CTL2_u
{
    struct I2S_CTL2_s
    {
        U32 MUTE_I2S0_DATA	    : 1;
        U32 _RSVD_0_         	: 1;
        U32 MUTE_I2S1_DATA	    : 1;
        U32 _RSVD_1_         	: 29;
    } field;
    U32 reg;
} I2S_CTL2_t;

 /* offset 0x0C */
typedef union I2S0_SET_u
{
    struct I2S0_SET_s
    {
        U32         EX_TX_LR_DATA		: 1;
        U32         EX_RX_LR_DATA       : 1;
        U32         I2S_BCLK_24B_MODE 	: 1;
        U32         SET_I2S_M_FS 	    : 1;
        U32         RX_SDI_LATCH_PHASE 	: 1;
        U32         _RSVD_0_       		: 1;
		U32         I2S_AUTX_WORD_LEN   : 2;
		U32         AURX_I2S_WORD_LEN   : 2;
		U32         I2S_DATA_WORD_LEN   : 2;
		U32         I2S_BIT_FORMAT      : 2;
		AU_I2S_MODE I2S_TR_MODE_CTL     : 2;
		U32         _RSVD_1_       		: 16;
    } field;
    U32 reg;
} I2S0_SET_t;

 /* offset 0x10 */
typedef union I2S1_SET_u
{
    struct I2S1_SET_s
    {
        U32         EX_TX_LR_DATA		: 1;
        U32         EX_RX_LR_DATA       : 1;
        U32         I2S_SDI_TIME 	    : 1;
        U32         I2S_SDO_TIME 	    : 1;
        U32         _RSVD_0_       		: 2;
		U32         I2S_AUTX_WORD_LEN   : 2;
		U32         AURX_I2S_WORD_LEN   : 2;
		U32         I2S_DATA_WORD_LEN   : 2;
		U32         I2S_BIT_FORMAT      : 2;
		AU_I2S_MODE I2S_TR_MODE_CTL     : 2;
		U32 _RSVD_1_       		: 16;
    } field;
    U32 reg;
} I2S1_SET_t;

typedef struct
{
	I2S_CTL0_t				        CTL0;		   	//offset 0x00
	I2S_CTL1_t					    CTL1;			//offset 0x04
	I2S_CTL2_t					    CTL2;			//offset 0x08
	I2S0_SET_t					    SET0;	        //offset 0x0C
	I2S1_SET_t					    SET1;	        //offset 0x10
} I2S_s;

EXTERN VOLATILE I2S_s I2S;

#endif
