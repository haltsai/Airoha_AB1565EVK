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

#ifdef MTK_WWE_ENABLE

#include "preroll.h"
#include "dsp_feature_interface.h"
#include "FreeRTOS.h"
/*define Debug option*/
//#define PREROLL_DEBUG
//#define PREROLL_EXTRA_DEBUG

#ifdef PREROLL_DEBUG
void vad_preroll_debug(void)
{
    //printf ring buffer data structure
    DSP_MW_LOG_I("********************************", 0);
    DSP_MW_LOG_I("handler:p_buffer = 0x%08x\r\n", 1, g_preroll_handler.p_buffer);
    DSP_MW_LOG_I("handler:buffer_size = %d\r\n", 1, g_preroll_handler.buffer_size);
    DSP_MW_LOG_I("handler:p_read = %d\r\n", 1, g_preroll_handler.p_read);
    DSP_MW_LOG_I("handler:p_write = %d\r\n", 1, g_preroll_handler.p_write);
    DSP_MW_LOG_I("handler:data_length = %d\r\n", 1, g_preroll_handler.data_length);
    DSP_MW_LOG_I("********************************", 0);
}
#endif

/*pre-roll buffer manage handler*/
static volatile preroll_handler_t g_preroll_handler;
static void *g_para = NULL;
static volatile preroll_extra_handler_t g_preroll_extra_handler;
extern void *g_p_wwe_extra_preroll_buffer;

/*pre-roll buffer allocate*/
__attribute__((aligned(4))) static volatile U8 g_preroll_buffer[PREROLL_BUFFER_SIZE];

/*reset & clear pre-roll buffer and state machine*/
preroll_status_t vad_preroll_reset(VOID *para)
{
    g_para = para;
    memset((void *)&g_preroll_handler, 0, sizeof(preroll_handler_t));

    /*init handler ring buffer size*/
    g_preroll_handler.buffer_size = sizeof(g_preroll_buffer);

    /*buffer_size validation check*/
    if (g_preroll_handler.buffer_size % (g_wwe_frame_size * 2)) {
        DSP_MW_LOG_I("[preroll][vad_preroll_reset]Invalid buffer_size!", 0);
        configASSERT(0);
    }

    /*init handler ring buffer address*/
    g_preroll_handler.p_buffer = (U8 *)g_preroll_buffer;

    /*init read pointer*/
    g_preroll_handler.p_read = 0;

    /*init write pointer*/
    g_preroll_handler.p_write = VAD_PREROLL_FRAME_NUMBER * 2 * g_wwe_frame_size;

    /*init data length*/
    g_preroll_handler.data_length = g_preroll_handler.p_write - g_preroll_handler.p_read;

    /*clear data buffer*/
    memset(g_preroll_handler.p_buffer, 0, g_preroll_handler.data_length);

    g_preroll_extra_handler.p_buffer = g_p_wwe_extra_preroll_buffer;
    g_preroll_extra_handler.buffer_size = PREROLL_BUFFER_SIZE;
    g_preroll_extra_handler.p_read = 0;
    g_preroll_extra_handler.p_write = 0;
    g_preroll_extra_handler.data_length = 0;
    memset(g_preroll_extra_handler.p_buffer, 0, PREROLL_BUFFER_SIZE);

    return PREROLL_STATUS_OK;
}

bool is_vad_preroll_extra_buffer_full(void)
{
#ifdef PREROLL_EXTRA_DEBUG
DSP_MW_LOG_I("[preroll_extra][is_vad_preroll_extra_buffer_full]is_full = %d", 1, (g_preroll_extra_handler.buffer_size == g_preroll_extra_handler.data_length));
#endif
    return (g_preroll_extra_handler.buffer_size == g_preroll_extra_handler.data_length);
}

preroll_status_t vad_preroll_extra_buffer_read(U8 *read_buf, U32 read_length)
{
    if(g_preroll_extra_handler.data_length < read_length) {
        return PREROLL_STATUS_ERROR;
    }
    memcpy(read_buf, g_preroll_extra_handler.p_buffer + g_preroll_extra_handler.p_read, read_length);
    g_preroll_extra_handler.p_read += read_length;
    if(g_preroll_extra_handler.p_read == g_preroll_extra_handler.buffer_size) {
        g_preroll_extra_handler.p_read = 0;
    }
    g_preroll_extra_handler.data_length -= read_length;
#ifdef PREROLL_EXTRA_DEBUG
    DSP_MW_LOG_I("[preroll_extra][vad_preroll_extra_buffer_read]p_read = 0x%08x, data_length = 0x%08x", 2, g_preroll_extra_handler.p_read, g_preroll_extra_handler.data_length);
#endif

    return PREROLL_STATUS_OK;
}


preroll_status_t vad_preroll_extra_buffer_write_done(U32 write_length)
{
    g_preroll_extra_handler.p_write += write_length;
    if(g_preroll_extra_handler.p_write == g_preroll_extra_handler.buffer_size) {
        g_preroll_extra_handler.p_write = 0;
    }
    g_preroll_extra_handler.data_length += write_length;
#ifdef PREROLL_EXTRA_DEBUG
    DSP_MW_LOG_I("[preroll_extra][vad_preroll_extra_buffer_write_done]p_write = 0x%08x, data_length = 0x%08x", 2, g_preroll_extra_handler.p_write, g_preroll_extra_handler.data_length);
#endif
    return PREROLL_STATUS_OK;
}


/*read data from the pre-roll buffer,read_length is byte unit*/
preroll_status_t vad_preroll_read_data(U8 *read_buf, U32 read_length)
{
    U32 length = 0;
    U32 p_read = g_preroll_handler.p_read;
    U32 buffer_size = g_preroll_handler.buffer_size;
    U8 *p_buffer = g_preroll_handler.p_buffer;

    /*read_length validation check*/
    if (read_length % (g_wwe_frame_size * 2)) {
        DSP_MW_LOG_I("[preroll][vad_preroll_read_data]Invalid read_length!", 0);
        configASSERT(0);
    }

    if (read_length > g_preroll_handler.data_length) {
        return PREROLL_STATUS_ERROR;
    }

    /*read data length not lead to buffer roll-back*/
    if ((p_read + read_length) <= buffer_size) {
        memcpy(read_buf, p_buffer + p_read, read_length);
    } else {
        /*read data length lead to buffer roll-back*/
        /*calculate the buffer remain length*/
        length = buffer_size - p_read;

        /*copy remain buffer to read buffer*/
        memcpy(read_buf, p_buffer + p_read, length);

        /*copy buffer roll-back address data to read buffer*/
        memcpy(read_buf + length, p_buffer, read_length - length);
    }

    /*update current ring buffer write pointer*/
    g_preroll_handler.p_read = (g_preroll_handler.p_read + read_length) % buffer_size;

    /*update current valid data length*/
    g_preroll_handler.data_length -= read_length;

    return PREROLL_STATUS_OK;
}

/*write data to ring buffer,if ring buffer is close to full,send 1 frame to CM4*/
/*write_length should align frame size*/
preroll_status_t vad_preroll_write_data(U8 *write_buf, U32 write_length)
{
    U32 length = 0;
    U32 p_write = g_preroll_handler.p_write;
    U32 buffer_size = g_preroll_handler.buffer_size;
    U8 *p_buffer = g_preroll_handler.p_buffer;
    short *mic_buf_1 = stream_function_get_inout_buffer(g_para, 1);

    /*write_length validation check*/
    if ((write_length % (g_wwe_frame_size * 2)) || (write_length > (g_preroll_handler.buffer_size - g_preroll_handler.data_length))) {
        DSP_MW_LOG_I("[preroll][vad_preroll_write_data]Invalid write_length!", 0);
        configASSERT(0);
    }

    /*write data length not lead to buffer roll-back*/
    if ((p_write + write_length) <= buffer_size) {
        memcpy(p_buffer + p_write, write_buf, write_length);
    } else {
        /*write data length lead to buffer roll-back*/
        /*calculate the buffer remain length*/
        length = buffer_size - p_write;

        /*copy write data to remain buffer*/
        memcpy(p_buffer + p_write, write_buf, length);

        /*copy remain write data to buffer roll-back address*/
        memcpy(p_buffer, write_buf + length, write_length - length);
    }

    /*update current ring buffer write pointer*/
    g_preroll_handler.p_write = (g_preroll_handler.p_write + write_length) % buffer_size;

    if (g_preroll_handler.compensation_number < VAD_PREROLL_FRAME_NUMBER) {
        /*compensite the dummy pre-roll data, to make sure CM4 receive no extra silent frames*/
        g_preroll_handler.p_read += write_length;
        g_preroll_handler.compensation_number++;
    } else {
        /*update current valid data length*/
        g_preroll_handler.data_length += write_length;
    }

    /*check data_length is reach the threshold,send 1 frame to CM4*/
    if (g_preroll_handler.data_length == g_preroll_handler.buffer_size) {

#ifdef PREROLL_DEBUG
        DSP_MW_LOG_I("[preroll][vad_preroll_write_data] threshold is reach,send data to CM4\r\n", 0);
        //vad_preroll_debug();
#endif
        vad_preroll_read_data((U8 *)g_preroll_extra_handler.p_buffer + g_preroll_extra_handler.p_write, g_wwe_frame_size * 2);
        vad_preroll_extra_buffer_write_done(g_wwe_frame_size * 2);
        //extra buffer full, the put the oldest frame to mic_buf_1
        if(is_vad_preroll_extra_buffer_full() == true) {
            vad_preroll_extra_buffer_read((U8 *)mic_buf_1, g_wwe_frame_size * 2);
            if(true == g_is_wwe_success) {
                stream_function_modify_output_size(g_para, g_wwe_frame_size * 2);
            } else {
                stream_function_modify_output_size(g_para, 0);
            }
        }else {
            stream_function_modify_output_size(g_para, 0);
        }
    } else {
        stream_function_modify_output_size(g_para, 0);
    }

    return PREROLL_STATUS_OK;
}

/*forward ring buffer write pointer to read data,this API not change the write point*/
/*forward_length should align frame size*/
preroll_status_t vad_preroll_forward_data(U32 *read_addr, U32 forward_length)
{
    /*forward_length validation check*/
    if ((forward_length % (g_wwe_frame_size * 2)) || (forward_length > g_preroll_handler.data_length)) {
        DSP_MW_LOG_I("[preroll][vad_preroll_forward_data]Invalid forward_length!", 0);
        configASSERT(0);
    }

    /* Get the pre-write address, based on the pre-write address to calculate the forward address
     * g_wwe_frame_size * 2 means the pre_write_address is the start addr to read the latest data,so need to forward 1 frame 
    */
    uint32_t pre_write_address = g_preroll_handler.p_read + g_preroll_handler.data_length - (g_wwe_frame_size * 2);

    *read_addr = (U32)g_preroll_handler.p_buffer + ((pre_write_address - forward_length) % g_preroll_handler.buffer_size);

#ifdef PREROLL_DEBUG
    DSP_MW_LOG_I("[preroll][vad_preroll_forward_data] forward addr = 0x%08x\r\n", 1, *read_addr);
#endif

    return PREROLL_STATUS_OK;
}

U32 vad_preroll_get_data_length(void)
{
    return g_preroll_handler.data_length + g_preroll_extra_handler.data_length;
}

#endif
