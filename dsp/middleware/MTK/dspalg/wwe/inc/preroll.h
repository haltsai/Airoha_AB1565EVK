/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#ifndef __PREROLL_H__
#define __PREROLL_H__

#ifdef MTK_WWE_ENABLE

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "types.h"
#include "dsp_temp.h"
#include "dsp_sdk.h"
#include "wwe_interface.h"

/*define pre-roll buffer static size*/
#define PREROLL_BUFFER_SIZE         (32640) /*32k pre-roll buffer size*/
#define VAD_PREROLL_FRAME_NUMBER    FORWARD_FRAME_NUMBER

/*define pre-roll status*/
typedef enum {
    PREROLL_STATUS_OK = 0,
    PREROLL_STATUS_ERROR = -1,
} preroll_status_t;

/*define pre-roll manage handler*/
typedef struct {
    U8 *p_buffer;       //ring buffer address
    U32 buffer_size;    //ring buffer total size
    U32 p_read;         //ring buffer read pointer
    U32 p_write;        //ring buffer write pointer
    U32 data_length;    //ring buffer current data length
    U32 compensation_number; //Compensation length for VAD_PREROLL_FRAME_NUMBER
} preroll_handler_t;

typedef struct {
    U8 *p_buffer;       //ring buffer address
    U32 buffer_size;    //ring buffer total size
    U32 p_read;         //ring buffer read pointer
    U32 p_write;        //ring buffer write pointer
    U32 data_length;    //ring buffer current data length
} preroll_extra_handler_t;

preroll_status_t vad_preroll_reset(VOID *para);
preroll_status_t vad_preroll_read_data(U8 *read_buf, U32 read_length);
preroll_status_t vad_preroll_write_data(U8 *write_buf, U32 write_length);
preroll_status_t vad_preroll_forward_data(U32 *read_addr, U32 forward_length);
U32 vad_preroll_get_data_length(void);

bool is_vad_preroll_extra_buffer_full(void);
preroll_status_t vad_preroll_extra_buffer_read(U8 *read_buf, U32 read_length);
preroll_status_t vad_preroll_extra_buffer_write_done(U32 write_length);

#endif//MTK_WWE_ENABLE
#endif

