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
 
#ifndef _DSP_DRV_DBG_H_
#define _DSP_DRV_DBG_H_

#include "types.h"
#include "dsp_drv_dfe.h"


/******************************************************************************
 * Constant define
 ******************************************************************************/


/******************************************************************************
 * Constant define
 ******************************************************************************/
    
#define DBG_BUF_SIZE (7)
#define DUMP_HALF_BUFSIZE (9)





/******************************************************************************
 * Enumerations
 ******************************************************************************/


typedef struct
{
    BOOL DbgOutEnable;
    U32 Mask[2];

    U32 DUMP_ID;
    U32 DUMP_IDRemainSize;
    U32 BufWo;
    U32 BufRo;
    U32 BufCnt;
}   DBG_CTRL_STRU;

typedef union S32_OUTDATA_TYPE_union
{
    struct S32_OUTDATA_TYPE_stru
    {
        U16 _rsvd_      :8;
        U16 DebugID     :8;
        S16 OutData;
    } Field;

    S32 Reg;

} S32_OUTDATA_t;

typedef enum 
{
    Dump_DA_Out = 0,
    Dump_I2S_MS_Out,
    Dump_I2S_SL_Out,
    Dump_SPDIF_Out,
    Dump_USB__Out
}Dump_output_sel_enum_s;



/******************************************************************************
 * External Global Variables
 ******************************************************************************/


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
   EXTERN VOID DUMP_DBG_SFR_INIT( Dump_output_sel_enum_s DUMP_OUTPUT_SEL); 
   EXTERN VOID DUMP_DBG_BUFFER_INIT (VOID);
   EXTERN VOID Debug_DataIn (S16* InBuf, U16 DataLength, U16 DebugID);
   EXTERN VOID Debug_DataOut (VOID);
   EXTERN VOID DSP_oDFE_DBGIsrHandler(VOID);


#endif /* _DSP_DRV_DBG_H_ */

