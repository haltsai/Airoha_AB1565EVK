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

#ifndef  __CLK_SKEW_PORTABLE_H__
#define  __CLK_SKEW_PORTABLE_H__

#ifdef MTK_BT_CLK_SKEW_USE_PIC
#include "skew_ctrl.h"

#ifdef MTK_BT_HFP_SPE_ALG_V2
typedef void (*p_skew_ctrl_init)(skew_ctrl_t *state,  U16 bits, U16 skew_io_mode, U16 order);
typedef void (*p_skew_ctrl_set_input_framesize)(skew_ctrl_t *state, U16 framsize);
typedef S16 (*p_skew_ctrl_process)(skew_ctrl_t *state, void *in_buf_ptr, U16 *in_byte_cnt, void *ou_buf_ptr, U16 *ou_byte_cnt, U16 skew_comp);
#else
typedef void (*p_skew_ctrl_init)(skew_ctrl_t *state, U16 channel, U16 bits, U16 skew_io_mode, U16 order);
typedef void (*p_skew_ctrl_set_input_framesize)(skew_ctrl_t *state, U16 framsize);
typedef void (*p_skew_ctrl_process)(skew_ctrl_t *state, void *in_buf_ptr, U16 *in_byte_cnt, void *ou_buf_ptr, U16 *ou_byte_cnt, U16 skew_comp);
#endif

extern void *clk_skew_import_parameters[];

/*for export parameters*************************************************/
extern void *clk_skew_export_parameters[];

// {skew_ctrl_init, skew_ctrl_process};

#define skew_ctrl_init                  ((p_skew_ctrl_init)clk_skew_export_parameters[0])
#define skew_ctrl_set_input_framesize   ((p_skew_ctrl_set_input_framesize)clk_skew_export_parameters[1])
#define skew_ctrl_process               ((p_skew_ctrl_process)clk_skew_export_parameters[2])

#endif

#endif


