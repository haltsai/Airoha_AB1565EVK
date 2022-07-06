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
 
#ifndef _DSP_TEMP_H_
#define _DSP_TEMP_H_

#include "types.h"
#include "syslog.h"
#include <string.h>

#define DSP_USE_MSGID_SEND_LOG
#ifdef DSP_USE_MSGID_SEND_LOG
#define DSP_MW_LOG_E(_message, arg_cnt, ...)  LOG_MSGID_E(dsp_mw,_message, arg_cnt, ##__VA_ARGS__)
#define DSP_MW_LOG_W(_message, arg_cnt, ...)  LOG_MSGID_W(dsp_mw,_message, arg_cnt, ##__VA_ARGS__)
#define DSP_MW_LOG_I(_message, arg_cnt, ...)  LOG_MSGID_I(dsp_mw,_message, arg_cnt, ##__VA_ARGS__)
#define DSP_MW_LOG_D(_message, arg_cnt, ...)  LOG_MSGID_D(dsp_mw,_message, arg_cnt, ##__VA_ARGS__)
#else
#define DSP_MW_LOG_E(_message, arg_cnt, ...)  LOG_E(dsp_mw,_message, ##__VA_ARGS__)
#define DSP_MW_LOG_W(_message, arg_cnt, ...)  LOG_W(dsp_mw,_message, ##__VA_ARGS__)
#define DSP_MW_LOG_I(_message, arg_cnt, ...)  LOG_I(dsp_mw,_message, ##__VA_ARGS__)
#define DSP_MW_LOG_D(_message, arg_cnt, ...)  LOG_D(dsp_mw,_message, ##__VA_ARGS__)
#endif

/******************************************************************************
 * External Global Variables
 ******************************************************************************/


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
EXTERN VOID Audio_Setup(VOID);
EXTERN VOID DSP_SetDefaultAfePara(VOID);


/******************************************************************************
 * Inline Functions
 ******************************************************************************/




#endif /* _DSP_TEMP_H_ */

