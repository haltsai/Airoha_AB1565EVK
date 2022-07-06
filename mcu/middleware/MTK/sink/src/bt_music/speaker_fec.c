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

#include "bt_sink_srv.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_utils.h"
#include "speaker_fec.h"
#include "hal_pmu.h"
#include "hal_dvfs_internal.h"
#include "hal_audio_internal.h"
#include "bt_avm.h"

#define BLK_NUM (4)
#define SPEAKER_FEC_RECIVE_MEDIA_DATA_TYPE 0
#define SPEAKER_FEC_RECEIVE_FEC_DATA_TYPE   1
#define SPEAKER_FEC_GEN_CCNI_HEADER(type, length)   (((type)<<16) | length)

#define SPEAKER_FEC_LOST_DATA_LENGTH        (700)
#define SPEAKER_FEC_SHARE_BUFFER_LENGTH     (700)
//#define SPEAKER_FEC_DEBUG_ENABLE


static speaker_fec_contex_t g_speaker_fec_cntx;
static speaker_fec_contex_t *speaker_fec_get_fec_context(void);

// GF256 with POLY 0x11d, TOPBIT 0x100, GEN 2
static const uint8_t gf256_log[256] = {
0x00, 0xFF, 0x01, 0x19, 0x02, 0x32, 0x1A, 0xC6, 0x03, 0xDF, 0x33, 0xEE, 0x1B, 0x68, 0xC7, 0x4B, 
0x04, 0x64, 0xE0, 0x0E, 0x34, 0x8D, 0xEF, 0x81, 0x1C, 0xC1, 0x69, 0xF8, 0xC8, 0x08, 0x4C, 0x71, 
0x05, 0x8A, 0x65, 0x2F, 0xE1, 0x24, 0x0F, 0x21, 0x35, 0x93, 0x8E, 0xDA, 0xF0, 0x12, 0x82, 0x45, 
0x1D, 0xB5, 0xC2, 0x7D, 0x6A, 0x27, 0xF9, 0xB9, 0xC9, 0x9A, 0x09, 0x78, 0x4D, 0xE4, 0x72, 0xA6, 
0x06, 0xBF, 0x8B, 0x62, 0x66, 0xDD, 0x30, 0xFD, 0xE2, 0x98, 0x25, 0xB3, 0x10, 0x91, 0x22, 0x88, 
0x36, 0xD0, 0x94, 0xCE, 0x8F, 0x96, 0xDB, 0xBD, 0xF1, 0xD2, 0x13, 0x5C, 0x83, 0x38, 0x46, 0x40, 
0x1E, 0x42, 0xB6, 0xA3, 0xC3, 0x48, 0x7E, 0x6E, 0x6B, 0x3A, 0x28, 0x54, 0xFA, 0x85, 0xBA, 0x3D, 
0xCA, 0x5E, 0x9B, 0x9F, 0x0A, 0x15, 0x79, 0x2B, 0x4E, 0xD4, 0xE5, 0xAC, 0x73, 0xF3, 0xA7, 0x57, 
0x07, 0x70, 0xC0, 0xF7, 0x8C, 0x80, 0x63, 0x0D, 0x67, 0x4A, 0xDE, 0xED, 0x31, 0xC5, 0xFE, 0x18, 
0xE3, 0xA5, 0x99, 0x77, 0x26, 0xB8, 0xB4, 0x7C, 0x11, 0x44, 0x92, 0xD9, 0x23, 0x20, 0x89, 0x2E, 
0x37, 0x3F, 0xD1, 0x5B, 0x95, 0xBC, 0xCF, 0xCD, 0x90, 0x87, 0x97, 0xB2, 0xDC, 0xFC, 0xBE, 0x61, 
0xF2, 0x56, 0xD3, 0xAB, 0x14, 0x2A, 0x5D, 0x9E, 0x84, 0x3C, 0x39, 0x53, 0x47, 0x6D, 0x41, 0xA2, 
0x1F, 0x2D, 0x43, 0xD8, 0xB7, 0x7B, 0xA4, 0x76, 0xC4, 0x17, 0x49, 0xEC, 0x7F, 0x0C, 0x6F, 0xF6, 
0x6C, 0xA1, 0x3B, 0x52, 0x29, 0x9D, 0x55, 0xAA, 0xFB, 0x60, 0x86, 0xB1, 0xBB, 0xCC, 0x3E, 0x5A, 
0xCB, 0x59, 0x5F, 0xB0, 0x9C, 0xA9, 0xA0, 0x51, 0x0B, 0xF5, 0x16, 0xEB, 0x7A, 0x75, 0x2C, 0xD7, 
0x4F, 0xAE, 0xD5, 0xE9, 0xE6, 0xE7, 0xAD, 0xE8, 0x74, 0xD6, 0xF4, 0xEA, 0xA8, 0x50, 0x58, 0xAF, 
};

static const uint8_t gf256_exp[256] = {
0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1D, 0x3A, 0x74, 0xE8, 0xCD, 0x87, 0x13, 0x26, 
0x4C, 0x98, 0x2D, 0x5A, 0xB4, 0x75, 0xEA, 0xC9, 0x8F, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 
0x9D, 0x27, 0x4E, 0x9C, 0x25, 0x4A, 0x94, 0x35, 0x6A, 0xD4, 0xB5, 0x77, 0xEE, 0xC1, 0x9F, 0x23, 
0x46, 0x8C, 0x05, 0x0A, 0x14, 0x28, 0x50, 0xA0, 0x5D, 0xBA, 0x69, 0xD2, 0xB9, 0x6F, 0xDE, 0xA1, 
0x5F, 0xBE, 0x61, 0xC2, 0x99, 0x2F, 0x5E, 0xBC, 0x65, 0xCA, 0x89, 0x0F, 0x1E, 0x3C, 0x78, 0xF0, 
0xFD, 0xE7, 0xD3, 0xBB, 0x6B, 0xD6, 0xB1, 0x7F, 0xFE, 0xE1, 0xDF, 0xA3, 0x5B, 0xB6, 0x71, 0xE2, 
0xD9, 0xAF, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88, 0x0D, 0x1A, 0x34, 0x68, 0xD0, 0xBD, 0x67, 0xCE, 
0x81, 0x1F, 0x3E, 0x7C, 0xF8, 0xED, 0xC7, 0x93, 0x3B, 0x76, 0xEC, 0xC5, 0x97, 0x33, 0x66, 0xCC, 
0x85, 0x17, 0x2E, 0x5C, 0xB8, 0x6D, 0xDA, 0xA9, 0x4F, 0x9E, 0x21, 0x42, 0x84, 0x15, 0x2A, 0x54, 
0xA8, 0x4D, 0x9A, 0x29, 0x52, 0xA4, 0x55, 0xAA, 0x49, 0x92, 0x39, 0x72, 0xE4, 0xD5, 0xB7, 0x73, 
0xE6, 0xD1, 0xBF, 0x63, 0xC6, 0x91, 0x3F, 0x7E, 0xFC, 0xE5, 0xD7, 0xB3, 0x7B, 0xF6, 0xF1, 0xFF, 
0xE3, 0xDB, 0xAB, 0x4B, 0x96, 0x31, 0x62, 0xC4, 0x95, 0x37, 0x6E, 0xDC, 0xA5, 0x57, 0xAE, 0x41, 
0x82, 0x19, 0x32, 0x64, 0xC8, 0x8D, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xDD, 0xA7, 0x53, 0xA6, 
0x51, 0xA2, 0x59, 0xB2, 0x79, 0xF2, 0xF9, 0xEF, 0xC3, 0x9B, 0x2B, 0x56, 0xAC, 0x45, 0x8A, 0x09, 
0x12, 0x24, 0x48, 0x90, 0x3D, 0x7A, 0xF4, 0xF5, 0xF7, 0xF3, 0xFB, 0xEB, 0xCB, 0x8B, 0x0B, 0x16, 
0x2C, 0x58, 0xB0, 0x7D, 0xFA, 0xE9, 0xCF, 0x83, 0x1B, 0x36, 0x6C, 0xD8, 0xAD, 0x47, 0x8E, 0x01, 
};

// mul for gf256
static uint8_t gf256_mul(uint8_t x, uint8_t y)
{
    if (x == 0 || y == 0) return 0;
    const uint32_t v = (uint32_t)gf256_log[x] + gf256_log[y];
    return gf256_exp[v % 255];
}

static uint8_t gf256_div(uint8_t x, uint8_t y)
{
    const uint32_t v = (uint32_t)255 - gf256_log[y] + gf256_log[x];
    return gf256_exp[v % 255];
}

static uint8_t gf256_add(uint8_t x, uint8_t y)
{
    return x ^ y;
}

static uint8_t gf256_sub(uint8_t x, uint8_t y)
{
    return x ^ y;
}

static uint8_t gf256_inv(uint8_t x)
{
    const uint8_t ret = gf256_div(1, x);
    return ret;
}

// x -= y
static void row_sub(uint8_t * x, const uint8_t * y, const uint32_t n)
{
    uint32_t i = 0;
    for (i = 0; i < n; ++i) {
        x[i] = gf256_sub(x[i], y[i]);
    }
}

#if 0
// x += y
static void row_add(uint8_t * x, const uint8_t * y, const uint32_t n)
{
    uint32_t i = 0;
    for (i = 0; i < n; ++i) {
        x[i] = gf256_add(x[i], y[i]);
    }
}
#endif

// x *= y
static void row_mul(uint8_t * x, const uint8_t y, const uint32_t n)
{
    uint32_t i = 0;
    for (i = 0; i < n; ++i) {
        x[i] = gf256_mul(x[i], y);
    }
}

// x = y
static void row_cpy(uint8_t * x, const uint8_t * y, const uint32_t n)
{
    memcpy(x, y, n);
}

const uint8_t aws_speaker_coe[][4] = {
    {0x01, 0x00, 0x00, 0x00},
    {0x00, 0x01, 0x00, 0x00},
    {0x00, 0x00, 0x01, 0x00},
    {0x00, 0x00, 0x00, 0x01},
    {0x01, 0x2A, 0x6C, 0xD7},
    {0x02, 0xBF, 0xD5, 0xEE},
    {0x03, 0xED, 0x40, 0x55},
    {0x04, 0xCC, 0xEF, 0x30},
    {0x05, 0x74, 0x12, 0x8C},
    {0x06, 0x8E, 0x6F, 0x7A},
    {0x07, 0x06, 0x32, 0x4F},
    {0x08, 0xED, 0xAB, 0x23},
    {0x09, 0x30, 0xD6, 0xB8},
    {0x0A, 0x47, 0xF0, 0x55},
    {0x0B, 0x90, 0x10, 0xDB},
    {0x00, 0x00, 0x00, 0x00}
};

static int32_t gaussian_elimination_48(uint8_t aws_verify[][8])
{
    const uint32_t n = 4;
    const uint32_t k = n << 1;

    uint32_t i = 0, j = 0;
    uint8_t tmp[k];
    for (i = 0; i < n; ++i) {
        for (j = 0; j < i; ++j) {
            row_cpy(tmp, aws_verify[j], k);
            row_mul(tmp, aws_verify[i][j], k);
            row_sub(aws_verify[i], tmp, k);
        }
        if (aws_verify[i][i] == 0) return -1;
        row_mul(aws_verify[i], gf256_inv(aws_verify[i][i]), k);
    }

    for (i = 0; i < n; ++i) {
        for (j = i + 1; j < n; ++j) {
            row_cpy(tmp, aws_verify[n - 1 - i], k);
            row_mul(tmp, aws_verify[n - 1 - j][n - 1 - i], k);
            row_sub(aws_verify[n - 1 - j], tmp, k);
        }
    }

    return 0;
}

static void speaker_fec_dvfs_lock()
{
#ifdef HAL_DVFS_416M_SOURCE
    hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M,HAL_DVFS_LOCK);
#elif defined(HAL_DVFS_312M_SOURCE)
    hal_dvfs_lock_control(DVFS_156M_SPEED, HAL_DVFS_LOCK);
#endif
}

static void speaker_fec_dvfs_unlock()
{
#ifdef HAL_DVFS_416M_SOURCE
    hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M,HAL_DVFS_UNLOCK);
#elif defined(HAL_DVFS_312M_SOURCE)
    hal_dvfs_lock_control(DVFS_156M_SPEED, HAL_DVFS_UNLOCK);
#endif
}

static const uint8_t *aws_speaker_find_coe(const uint32_t poly_no)
{
    uint32_t i = 4;
    while (aws_speaker_coe[i][0] != 0 && aws_speaker_coe[i][0] != poly_no) ++i;
    if (aws_speaker_coe[i][0] == 0) return NULL;
    return aws_speaker_coe[i];
}

static uint32_t speaker_fec_get_max_data_length(speaker_fec_contex_t *fec_cntx)
{
    uint32_t max_length = 0;
    uint8_t i = 0;

    for(i = 0; i < fec_cntx->data_count; i++) {
        if(max_length < fec_cntx->media_data[i].data_size) {
            max_length = fec_cntx->media_data[i].data_size;
        }
    }

    return max_length;
}

uint8_t *speaker_fec_get_share_buffer(void)
{
    return (uint8_t *)hal_audio_query_fec_share_info();
}

static uint8_t *speaker_fec_find_unused_share_buffer(uint8_t *fec_des_data, uint32_t share_buffer_count)
{
    uint8_t *fec_gen_data = fec_des_data;
    uint32_t i = 0;
    for(i = 0; i < share_buffer_count; i++) {
        uint32_t used_flag = *(uint32_t *)(fec_gen_data);
        if(used_flag) {
            fec_gen_data += SPEAKER_FEC_SHARE_BUFFER_LENGTH;
        } else {
            return fec_gen_data;
        }
    }
    return NULL;
}

static uint8_t *speaker_fec_get_real_addr(uint8_t *addr, uint32_t audio_end_addr, uint32_t buffer_length);

static uint32_t generate_fec_packet(uint32_t poly_no, uint8_t **fec_gen_data)
{
    speaker_fec_contex_t *fec_cntx = speaker_fec_get_fec_context();
    speaker_fec_media_data_t *media_data_list = fec_cntx->media_data;
    uint32_t max_fec_length = speaker_fec_get_max_data_length(fec_cntx);
    uint8_t *fec_des_data = (uint8_t *)hal_audio_query_fec_share_info();
    fec_des_data = speaker_fec_find_unused_share_buffer(fec_des_data, SPEAKER_FEC_SHARE_BUFFER_COUNT);
    if (!fec_des_data) {
        bt_sink_srv_report_id("[FEC][ERROR] Couldn`t found free share buffer,  poly_no: 0x%02x", 1, poly_no);
        return 0;
    }
    bt_sink_srv_assert(fec_des_data && "Couldn`t found free share buffer");
    uint32_t fec_gen_begin, fec_gen_end;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &fec_gen_begin);

    bt_sink_srv_report_id("poly_no 0x%02x ", 1, poly_no);
    const uint8_t * coe = aws_speaker_find_coe(poly_no);
    if (coe == NULL) return 0;

    // generate data
    n9_dsp_share_info_t *share_buffer = hal_audio_query_bt_audio_dl_share_info();
    uint32_t audio_end_addr = share_buffer->start_addr + share_buffer->length;
    uint32_t i = 4;
    for (i = 0; i < max_fec_length; ++i) {
        uint8_t b = 0;
        uint32_t j = 0;
        for (j = 0; j < 4; ++j) {
            uint8_t v = 0;
            uint8_t *data_addr = NULL;
            
            if (i < media_data_list[j].data_size) {
                data_addr = speaker_fec_get_real_addr(media_data_list[j].data + i, audio_end_addr, share_buffer->length);
                v = *data_addr;
#ifdef SPEAKER_FEC_DEBUG_ENABLE
                if(i < 10) {
                    bt_sink_srv_report_id("sn:0x%04x, data[%d]:0x%02x", 3, media_data_list[j].sequence_number, i, v);
                }
#endif
            }
            b = gf256_add(b, gf256_mul(v, coe[j]));
        }
        fec_des_data[i+4] = b;
    }
    *fec_gen_data = fec_des_data;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &fec_gen_end);  
    bt_sink_srv_report_id("[FEC] gen data, cost: %d ms, gen_fec_len:%d, start_seq:0x%04x, end_seq:0x%04x", 4,
                         (fec_gen_end-fec_gen_begin)  * 1000 / 32768, max_fec_length,
                         media_data_list[0].sequence_number, media_data_list[fec_cntx->data_count-1].sequence_number);
    return max_fec_length;
}

static uint8_t *speaker_fec_get_real_addr(uint8_t *addr, uint32_t audio_end_addr, uint32_t buffer_length)
{
    void *real_addr = addr;
    if(((uint32_t)addr) >= audio_end_addr) {
        real_addr = addr - buffer_length;
    }
    return real_addr;
}

#ifdef SPEAKER_FEC_DEBUG_ENABLE
static uint8_t test_recover_data[SPEAKER_FEC_LOST_DATA_LENGTH];
static uint8_t test_fec_data[SPEAKER_FEC_LOST_DATA_LENGTH];
#endif
static void speaker_fec_get_group_info(speaker_fec_group_info_t *group_info, speaker_fec_ccni_received_data_t *ccni_received_data);

static void speaker_fec_fill_lost_packet_pcb_header(bt_sink_srv_audio_pcb_type_t *pcb_header, uint8_t *lost_media_data)
{
    printf("speaker_fec_fill_lost_packet_pcb_header, len:0x%02x\r\n", pcb_header->size);
    uint8_t pcb_header_len = sizeof(bt_sink_srv_audio_pcb_type_t);
    uint8_t pcb_data_remained[4];
    bt_sink_srv_audio_pcb_type_t *remained_pcb = (bt_sink_srv_audio_pcb_type_t *)pcb_data_remained;
    n9_dsp_share_info_t *share_buffer = hal_audio_query_bt_audio_dl_share_info();
    uint32_t audio_end_addr = share_buffer->start_addr + share_buffer->length;

    remained_pcb->size = SPEAKER_FEC_LOST_DATA_LENGTH - pcb_header->size - (pcb_header_len<<1);
    remained_pcb->state = SPEAKER_FEC_PCB_STATE_SKIP;
    remained_pcb->data_size = 0;
    uint8_t *data_addr = speaker_fec_get_real_addr(lost_media_data + pcb_header->size + pcb_header_len, audio_end_addr, share_buffer->length);
    memcpy(data_addr, remained_pcb, 4);

    data_addr = speaker_fec_get_real_addr(lost_media_data, audio_end_addr, share_buffer->length);
    bt_sink_srv_audio_pcb_type_t *lost_data_pcb = (bt_sink_srv_audio_pcb_type_t *)data_addr;
    lost_data_pcb->data_size = pcb_header->data_size;
    lost_data_pcb->size = pcb_header->size;
    lost_data_pcb->state = SPEAKER_FEC_PCB_STATE_USED;
}

//#define SPEAKER_DUMP_RECOVER_DATA

#ifdef SPEAKER_DUMP_RECOVER_DATA
uint8_t temp_data[4][700];
uint32_t g_fec_tag = 0;
#endif

int32_t recover_loss_packet(speaker_fec_lost_data_t *lost_parameters, speaker_fec_media_data_t *media_data_list, speaker_fec_gen_fec_group_t *fec_rec_group)
{
    // prepare coe
    uint8_t fec_coe[4][8];
    uint8_t fec_pol[4] = {0, 0, 0, 0};
    uint8_t pcb_header_array[4][4];
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t fec_rec_begin, fec_rec_end;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &fec_rec_begin);

    n9_dsp_share_info_t *share_buffer = hal_audio_query_bt_audio_dl_share_info();
    uint32_t audio_end_addr = share_buffer->start_addr + share_buffer->length;
    speaker_fec_ccni_received_data_t *fec_gen_list = fec_rec_group->fec_gen_data;
    uint8_t *data_addr = NULL;

    /*Save pcb header to temp array*/
    for (i = 0; i < 4; ++i) {
        memset(fec_coe[i],0,8);
        fec_coe[i][i + 4] = fec_coe[i][i] = 1;
        data_addr = speaker_fec_get_real_addr(media_data_list[i].data, audio_end_addr, share_buffer->length);
        memcpy(pcb_header_array[i], data_addr, 4);
#ifdef SPEAKER_DUMP_RECOVER_DATA
        memset(temp_data[i], 0, 700);
#endif
    }

    /*Fill lost packet parameters*/
    bt_sink_srv_report_id("lost count:%d",1, lost_parameters->lost_data_count);
    for(i = 0; i < (lost_parameters->lost_data_count); i++) {
        speaker_fec_group_info_t group_info;
        speaker_fec_get_group_info(&group_info, &fec_gen_list[i]);
        uint8_t lost_index = lost_parameters->lost_data_index_list[i];

        /*Fill fec_coe */
        const uint8_t * coe = aws_speaker_find_coe(group_info.poly_num);
        
        bt_sink_srv_report_id("coe[0]:0x%02x, coe[1]:0x%02x, coe[2]:0x%02x, coe[3]:0x%02x, i:%d", 5, coe[0], coe[1], coe[2], coe[3], i);
        bt_sink_srv_report_id("sn:%d, poly_no:0x%08x",2, lost_index, group_info.poly_num);
        bt_sink_srv_assert(coe && "No COE for poly_no");
        memcpy(fec_coe[lost_index], coe, 4);

        /*Fill fec_pol*/
        fec_pol[lost_index] = group_info.poly_num;

        /*Fill length*/
        media_data_list[lost_index].data_size = fec_gen_list[i].seq_or_len;

        /*Fill fec gen data`s pcb header to temp array*/
        memcpy(pcb_header_array[lost_index], (uint8_t *)(fec_gen_list[i].data_addr) + 4, 4);

        /*Fill fec gen data to lost packet`s share buffer*/
        uint32_t sd_loop = (fec_gen_list[i].seq_or_len-4)>>2;
        uint32_t start_fec_addr = (uint32_t)(media_data_list[lost_index].data) + 4;
        uint32_t src_addr = (fec_gen_list[i].data_addr) + 8;
        uint32_t *data_point = NULL;
        bt_sink_srv_report_id("sd_loop:%d",1, sd_loop);
        uint32_t k = 0;
        for(k = 0; k < sd_loop; k++)
        {
            data_addr = speaker_fec_get_real_addr((uint8_t *)start_fec_addr, audio_end_addr, share_buffer->length);
            data_point = (uint32_t *)data_addr;
            *data_point = *(uint32_t *)src_addr;
            start_fec_addr += 4;
            src_addr += 4;
#ifdef SPEAKER_FEC_DEBUG_ENABLE
            if (k<10) {
                bt_sink_srv_report_id("*data_addr:0x%02x\r\n", 1, *data_addr);
            }
#endif
        }
#ifdef SPEAKER_FEC_DEBUG_ENABLE
        LOG_HEXDUMP_I(sink_srv, "FEC gen data", media_data_list[lost_index].data, 20);
#endif
    }
    
    bt_sink_srv_report_id("fec_pol[0]:0x%02x, fec_pol[1]:0x%02x, fec_pol[2]:0x%02x, fec_pol[3]:0x%02x, i:%d", 5, fec_pol[0], fec_pol[1], fec_pol[2], fec_pol[3], i);

    if (gaussian_elimination_48(fec_coe) < 0) {
        bt_sink_srv_report_id("Gaussian NG\n", 0);
        return -1;
    }
    // recover data
    for (i = 1; i < fec_gen_list[0].seq_or_len; ++i) {
        uint8_t a0,a1,a2,a3;
        if(i >= 4) {
            uint8_t *a0_addr = speaker_fec_get_real_addr(media_data_list[0].data + i, audio_end_addr, share_buffer->length);
            uint8_t *a1_addr = speaker_fec_get_real_addr(media_data_list[1].data + i, audio_end_addr, share_buffer->length);
            uint8_t *a2_addr = speaker_fec_get_real_addr(media_data_list[2].data + i, audio_end_addr, share_buffer->length);
            uint8_t *a3_addr = speaker_fec_get_real_addr(media_data_list[3].data + i, audio_end_addr, share_buffer->length);
            a0 = (i < media_data_list[0].data_size ? (*a0_addr) : 0);
            a1 = (i < media_data_list[1].data_size ? (*a1_addr) : 0);
            a2 = (i < media_data_list[2].data_size ? (*a2_addr) : 0);
            a3 = (i < media_data_list[3].data_size ? (*a3_addr) : 0);
        } else {
            a0 = pcb_header_array[0][i];
            a1 = pcb_header_array[1][i];
            a2 = pcb_header_array[2][i];
            a3 = pcb_header_array[3][i];
        }
        for(j = 0; j < 4; j++) {
            if(fec_pol[j] == 0) continue;
            if(i > 4) {
                bt_sink_srv_audio_pcb_type_t *used_pcb = (bt_sink_srv_audio_pcb_type_t *)pcb_header_array[j];
                if(i > (used_pcb->size + 4)) {
                    continue;
                }
            }
            const uint8_t b0 = gf256_mul(a0, fec_coe[j][4]);
            const uint8_t b1 = gf256_mul(a1, fec_coe[j][5]);
            const uint8_t b2 = gf256_mul(a2, fec_coe[j][6]);
            const uint8_t b3 = gf256_mul(a3, fec_coe[j][7]);
            uint8_t r = gf256_add(gf256_add(b0, b1), gf256_add(b2, b3));
#ifdef SPEAKER_DUMP_RECOVER_DATA
            temp_data[j][i] = r;
#endif
#ifdef SPEAKER_FEC_DEBUG_ENABLE
            if(i < 10) {
                bt_sink_srv_report_id("a0:0x%02x,a1:0x%02x,a2:0x%02x,a3:0x%02x, r:0x%02x, j:%d\r\n", 6, a0,a1,a2,a3,r,j);
            }
#endif
            if(i < 4) {
                pcb_header_array[j][i] = r;
            } else {
                data_addr = speaker_fec_get_real_addr(media_data_list[j].data + i, audio_end_addr, share_buffer->length);
                *data_addr = r;
            }
        }
    }

    for(i = 0; i < lost_parameters->lost_data_count; i++) {
        uint8_t lost_index = lost_parameters->lost_data_index_list[i];

        bt_sink_srv_audio_pcb_type_t *used_pcb = (bt_sink_srv_audio_pcb_type_t *)pcb_header_array[lost_index];
        speaker_fec_fill_lost_packet_pcb_header(used_pcb, media_data_list[lost_index].data);
        LOG_HEXDUMP_I(sink_srv, "FEC recover data", media_data_list[lost_index].data, 20);
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &fec_rec_end);  
    bt_sink_srv_report_id("[FEC] rec data, cost: %d ms", 1,
                         (fec_rec_end-fec_rec_begin)  * 1000 / 32768);
    return 0;
}

static speaker_fec_contex_t *speaker_fec_get_fec_context(void)
{
    return &g_speaker_fec_cntx;
}

static void speaker_fec_find_lost_packet(uint8_t start_index, uint8_t end_index, speaker_fec_lost_data_t *lost_data_list)
{
    speaker_fec_contex_t *fec_cntx = speaker_fec_get_fec_context();
    uint32_t i = 0;

    lost_data_list->lost_data_count = 0;
    for(i = start_index; i < end_index && i < fec_cntx->data_count; i++ ) {
        if(fec_cntx->media_data[i].packet_flag == SPEAKER_FEC_CCNI_RECEVIED_LOST_MEDIA_DATA) {
            lost_data_list->lost_data_index_list[lost_data_list->lost_data_count] = i;
            lost_data_list->lost_data_count++;
            bt_sink_srv_report_id("[FEC]lost index:%d, lost_data_count:%d", 2, i, lost_data_list->lost_data_count);
        }
    }
}

static speaker_fec_media_data_t *speaker_fec_find_recover_data_group(uint16_t start_seq, uint8_t *start_index, uint8_t *remain_count)
{
    speaker_fec_contex_t *fec_cntx = speaker_fec_get_fec_context();
    speaker_fec_media_data_t *start_addr = NULL;
    uint8_t i = 0;
    for(i = 0; i < fec_cntx->data_count; i++) {
        if(fec_cntx->media_data[i].sequence_number == start_seq) {
            *remain_count = fec_cntx->data_count - i;
            start_addr = &(fec_cntx->media_data[i]);
            *start_index = i;
            break;
        }
    }
    bt_sink_srv_report_id("[FEC]find_recover_data_group, start_seq:0x%04x, start_index:%d, remain_count:%d, start_addr:0x%08x", 4,
                         start_seq, *start_index, *remain_count, start_addr);
    return start_addr;
}

static void speaker_fec_get_group_info(speaker_fec_group_info_t *group_info, speaker_fec_ccni_received_data_t *ccni_received_data)
{
    memcpy(group_info, (void *)(ccni_received_data->data_addr + SPEAKER_FEC_SEQUENCE_NUMBER_GROUP_BASE_OFFSET), sizeof(speaker_fec_group_info_t));
}

static void speaker_fec_free_fec_gen_data_group(speaker_fec_gen_fec_group_t *free_fec_group)
{
    speaker_fec_ccni_received_data_t *fec_gen_data = free_fec_group->fec_gen_data;
    uint32_t fec_gen_count = free_fec_group->gen_fec_count;
    bt_sink_srv_assert(free_fec_group && "NULL free fec group");

    uint32_t i = 0;
    for(i = 0; i < fec_gen_count; i++) {
        uint32_t *used_flag = (uint32_t *)fec_gen_data[i].data_addr;
        *used_flag = 0;
        bt_sink_srv_report_id("[FEC]Free addr:0x%08x", 1, fec_gen_data[i].data_addr);
    }
    free_fec_group->start_seq = 0;
    free_fec_group->gen_fec_count = 0;
}

static void speaker_fec_recover_client_data(speaker_fec_gen_fec_group_t *fec_group)
{
    speaker_fec_contex_t *fec_cntx = speaker_fec_get_fec_context();
    speaker_fec_media_data_t *media_data_list = fec_cntx->media_data;
    speaker_fec_lost_data_t lost_data;
    uint8_t start_index = 0;
    uint8_t remain_count = 0;
    uint32_t i = 0;

    speaker_fec_media_data_t *recover_data_list = speaker_fec_find_recover_data_group(fec_group->start_seq, &start_index, &remain_count);
    if(recover_data_list) {
        if(remain_count >= SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT) {
            speaker_fec_find_lost_packet(start_index, start_index + SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT, &lost_data);
            for(i = 0; i < lost_data.lost_data_count; i++) {
                lost_data.lost_data_index_list[i] -= start_index;
            }
            if(lost_data.lost_data_count) {
                if(lost_data.lost_data_count <= fec_group->gen_fec_count) {
                    //pmu_vcore_lock_control(PMU_NORMAL, PMIC_VCORE_0P8_V, PMU_LOCK);
                    //hal_dvfs_lock_control("A2DP", DVFS_FULL_SPEED_104M, HAL_DVFS_LOCK);
                    speaker_fec_dvfs_lock();
                    int32_t ret = recover_loss_packet(&lost_data, recover_data_list, fec_group);
                    bt_sink_srv_assert((ret != -1) && "recover fail");
                    speaker_fec_free_fec_gen_data_group(fec_group);
                    //pmu_vcore_lock_control(PMU_NORMAL, PMIC_VCORE_0P8_V, PMU_UNLOCK);
                    //hal_dvfs_lock_control("A2DP", DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                    speaker_fec_dvfs_unlock();
                    remain_count -= SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT;
                }
            } else {
                speaker_fec_free_fec_gen_data_group(fec_group);
                remain_count -= SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT;
            }
        }
        memcpy(&media_data_list[0], &media_data_list[fec_cntx->data_count - remain_count], remain_count*sizeof(speaker_fec_media_data_t));
        fec_cntx->data_count = remain_count;
    } else {
        if(fec_cntx->data_count && (media_data_list[0].sequence_number < fec_group->start_seq)) {
            fec_cntx->data_count = 0;
        } else {
            speaker_fec_free_fec_gen_data_group(fec_group);

        }
    }
    bt_sink_srv_report_id("[FEC]start_seq:0x%04x, seq_count:%d", 2, fec_group->start_seq, fec_group->gen_fec_count);
}

static void speaker_fec_fill_recevied_media_info(speaker_fec_ccni_received_data_t *ccni_received_data)
{
    bt_sink_srv_audio_pcb_type_t *pcb_data = (bt_sink_srv_audio_pcb_type_t *)(ccni_received_data->data_addr);
    speaker_fec_contex_t *fec_cntx = speaker_fec_get_fec_context();
    speaker_fec_media_data_t *media_data = fec_cntx->media_data;

    if(fec_cntx->data_count && media_data[fec_cntx->data_count-1].sequence_number + 1 != ccni_received_data->seq_or_len) {
        /* check with previous packet to check whether is duplicate pakcet */
        if ((media_data[fec_cntx->data_count-1].sequence_number == ccni_received_data->seq_or_len)
            && (*((uint32_t *)media_data[fec_cntx->data_count-1].data) == ccni_received_data->data_addr)) {
            bt_sink_srv_report_id("[FEC][WARNNING] receive dumplicate data will be drop, seq:0x%04x", 1, ccni_received_data->seq_or_len);
            return;
        }
        fec_cntx->data_count = 0;
    } else if (fec_cntx->data_count >= SPEAKER_FEC_MAX_RECEVIED_RAW_DATA_COUNT) {
        fec_cntx->data_count--;
        bt_sink_srv_memcpy(&media_data[0], &media_data[1], fec_cntx->data_count * sizeof(speaker_fec_media_data_t));
    }

    uint8_t data_index = fec_cntx->data_count;
    media_data[data_index].sequence_number = ccni_received_data->seq_or_len;
    media_data[data_index].data = (uint8_t *)(ccni_received_data->data_addr);
    media_data[data_index].data_size = pcb_data->size + sizeof(bt_sink_srv_audio_pcb_type_t);
    media_data[data_index].packet_flag = ccni_received_data->data_type;
    bt_sink_srv_report_id("[FEC]speaker_fec_fill_recevied_media_info, seq:0x%04x, data_size:0x%04x,packet_flag:0x%08x, data_count:%d", 4,
                         media_data[data_index].sequence_number,
                         media_data[data_index].data_size,
                         media_data[data_index].packet_flag,
                         fec_cntx->data_count);
    fec_cntx->data_count++;
}


static speaker_fec_gen_fec_group_t *speaker_fec_fill_received_fec_gen_data(speaker_fec_ccni_received_data_t *recv_fec_gen_data)
{
    speaker_fec_contex_t *fec_cntx = speaker_fec_get_fec_context();
    speaker_fec_gen_fec_group_t *des_fec_group = &fec_cntx->fec_gen_group;
    speaker_fec_group_info_t group_info;
    speaker_fec_get_group_info(&group_info, recv_fec_gen_data);
    if(des_fec_group->gen_fec_count && des_fec_group->start_seq != group_info.base_sn) {
        speaker_fec_free_fec_gen_data_group(des_fec_group);
    }
    bt_sink_srv_assert(des_fec_group && "No free fec group");
    uint16_t fec_count = des_fec_group->gen_fec_count;
    /* filter duplicate FEC data according to poly_num */
    uint16_t idx = 0;
    speaker_fec_group_info_t exist_group_info;
    for (idx = 0; idx < fec_count; idx++) {
        speaker_fec_get_group_info(&exist_group_info, &(des_fec_group->fec_gen_data[idx]));
        if ((exist_group_info.poly_num == group_info.poly_num)
            && (exist_group_info.base_sn == group_info.base_sn)
            && (des_fec_group->fec_gen_data[idx].data_addr == recv_fec_gen_data->data_addr)) {
            bt_sink_srv_report_id("[FEC][WARNNING] duplicate speaker_fec_fill_received_fec_gen_data, seq:0x%04x, sn_count:0x%02x,poly_num:0x%02x, data_count:%d", 4,
                                 group_info.base_sn,
                                 group_info.sn_count,
                                 group_info.poly_num,
                                 des_fec_group->gen_fec_count);
            return des_fec_group;
        }
    }
    
    if(fec_count < SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT) {
        memcpy(&(des_fec_group->fec_gen_data[fec_count]), recv_fec_gen_data, sizeof(speaker_fec_ccni_received_data_t));
        des_fec_group->start_seq = group_info.base_sn;
        des_fec_group->gen_fec_count++;
        bt_sink_srv_report_id("[FEC]speaker_fec_fill_received_fec_gen_data, seq:0x%04x, sn_count:0x%02x,poly_num:0x%02x, data_count:%d", 4,
                             group_info.base_sn,
                             group_info.sn_count,
                             group_info.poly_num,
                             des_fec_group->gen_fec_count);
    } else {
        uint32_t *used_flag = (uint32_t *)recv_fec_gen_data->data_addr;
        *used_flag = 0;
        bt_sink_srv_report_id("[FEC]Free addr:0x%08x", 1, recv_fec_gen_data->data_addr);
    }
    return des_fec_group;
}

static void speaker_fec_handle_client_recevied_data(speaker_fec_ccni_received_data_t *ccni_received_data)
{
    if(ccni_received_data->data_type == SPEAKER_FEC_CCNI_RECEIVED_VALID_MEDIA_DATA
        || ccni_received_data->data_type == SPEAKER_FEC_CCNI_RECEVIED_LOST_MEDIA_DATA) {
        speaker_fec_fill_recevied_media_info(ccni_received_data);
    } else if (ccni_received_data->data_type == SPEAKER_FEC_CCNI_RECEVIED_FEC_DATA) {
        LOG_HEXDUMP_I(sink_srv, "recv fec data", (uint8_t *)ccni_received_data->data_addr, 20);
        speaker_fec_gen_fec_group_t *fec_group = speaker_fec_fill_received_fec_gen_data(ccni_received_data);
        assert(fec_group && "fec_group NULL");
        speaker_fec_recover_client_data(fec_group);
        //memcpy(pre_fec_data, ccni_received_data, sizeof(speaker_fec_ccni_received_data_t));
    }
}

static void speaker_fec_fill_fec_parameters(uint8_t *fec_gen_data, uint8_t poly_number, uint8_t seq_count, uint16_t base_seq)
{
    uint16_t *seq_base = (uint16_t *)(fec_gen_data + SPEAKER_FEC_SEQUENCE_NUMBER_GROUP_BASE_OFFSET);
    uint8_t *sequence_count = fec_gen_data + SPEAKER_FEC_SEQUENCE_NUMBER_GROUP_COUNT_OFFSET;
    uint8_t *poly_num = fec_gen_data + SPEAKER_FEC_POLY_NOMBER_OFFSET;
    uint32_t *fec_data_header = (uint32_t *)fec_gen_data;
    *fec_data_header = 0x00000001;
    *seq_base = base_seq;
    *sequence_count = seq_count;
    *poly_num = poly_number;
}

static uint8_t g_fec_count = SPEAKER_FEC_GEN_FEC_DATA_COUNT;

void speaker_fec_set_fec_gen_count(uint8_t fec_count)
{
    g_fec_count = fec_count;
    bt_sink_srv_report_id("[FEC]fec_count:%d", 1, g_fec_count);
}

extern void bt_driver_gen_fec_ccni_to_controller(uint32_t type, uint32_t addr);
static void speaker_fec_handle_agent_recevied_data(speaker_fec_ccni_received_data_t *ccni_received_data)
{
    speaker_fec_contex_t *fec_cntx = speaker_fec_get_fec_context();
    uint8_t *fec_gen_data = NULL;
    uint8_t poly_no = 0x01;
    uint32_t i = 0;

    speaker_fec_fill_recevied_media_info(ccni_received_data);
    if(fec_cntx->data_count == SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT) {
        speaker_fec_dvfs_lock();
        //pmu_vcore_lock_control(PMU_NORMAL, PMIC_VCORE_0P8_V, PMU_LOCK);
        //hal_dvfs_lock_control("A2DP", DVFS_FULL_SPEED_104M, HAL_DVFS_LOCK);
        for(i = 0; i < g_fec_count; i++) {
            uint32_t fec_len = generate_fec_packet(poly_no, &fec_gen_data);
            if(!fec_len) break;
            speaker_fec_fill_fec_parameters(fec_gen_data, poly_no, SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT, fec_cntx->media_data[0].sequence_number);
            uint32_t ccni_type_len = SPEAKER_FEC_GEN_CCNI_HEADER(SPEAKER_FEC_CCNI_TO_CONTROLLER_FEC_READY, fec_len);
            poly_no++;

            LOG_HEXDUMP_I(sink_srv, "FEC gen data", fec_gen_data, 20);
            bt_driver_gen_fec_ccni_to_controller(ccni_type_len, (uint32_t)fec_gen_data);
            bt_sink_srv_report_id("[FEC]ccni_type_len:0x%08x, data_addr:0x%08x", 2, ccni_type_len, fec_gen_data);
        }
        fec_cntx->data_count = 0;
        //pmu_vcore_lock_control(PMU_NORMAL, PMIC_VCORE_0P8_V, PMU_UNLOCK);
        //hal_dvfs_lock_control("A2DP", DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
        speaker_fec_dvfs_unlock();
    }
}


QueueHandle_t g_xQueue_fec;
#define SPEAKER_FEC_QUEUE_NUMBER 15


void speaker_fec_task(void *fec_data)
{
    g_xQueue_fec = xQueueCreate(SPEAKER_FEC_QUEUE_NUMBER, 2*sizeof(speaker_fec_ccni_received_data_t));
    speaker_fec_ccni_received_data_t fec_recv_data;
    bt_aws_mce_role_t role;
    while (1) {
        if (xQueueReceive(g_xQueue_fec, &fec_recv_data, portMAX_DELAY)) {
            role = bt_connection_manager_device_local_info_get_aws_role();
            if(role == BT_AWS_MCE_ROLE_AGENT) {
                speaker_fec_handle_agent_recevied_data(&fec_recv_data);
            } else if(role == BT_AWS_MCE_ROLE_CLINET) {
                speaker_fec_handle_client_recevied_data(&fec_recv_data);
            } else {
                assert(0 && "wrong aws role!");
            }
        }
    }
}

void speaker_fec_data_callback(void *fec_ccni_recv_data)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *fec_data = (uint32_t *)fec_ccni_recv_data;
    speaker_fec_ccni_received_data_t fec_ccni_data;
    fec_ccni_data.data_type = (fec_data[0] & 0xffff0000)>>16;
    fec_ccni_data.seq_or_len = (fec_data[0] & 0x0000ffff);
    fec_ccni_data.data_addr = fec_data[1];
    //xQueueSend(xQueue,pvItemToQueue,xTicksToWait);
    bt_sink_srv_report_id("[FEC]recv_ccni, data_type:0x%04x, seq_or_len:0x%04x,data_addr:0x%08x", 3,
        fec_ccni_data.data_type,
        fec_ccni_data.seq_or_len,
        fec_ccni_data.data_addr);
#ifdef SPEAKER_DUMP_RECOVER_DATA
    if (fec_ccni_data.data_type == SPEAKER_FEC_CCNI_RECEIVED_VALID_MEDIA_DATA) {
        if (g_fec_tag != *fec_data) {
            g_fec_tag = *fec_data;
        } else {
            bt_sink_srv_assert(0 && "duplicated data received");
        }
    }
#endif /* SPEAKER_DUMP_RECOVER_DATA */

    if(xQueueSendFromISR(g_xQueue_fec, &fec_ccni_data, &xHigherPriorityTaskWoken) != pdPASS) {
        bt_sink_srv_report_id("[FEC]recv_ccni, Queue full, just drop this message", 0);
        //assert(0&&"send msg fail");
    }
    if(xHigherPriorityTaskWoken)
    {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void speaker_fec_clear_share_buffer()
{
    uint32_t i = 0;
    uint8_t *share_buffer_addr = speaker_fec_get_share_buffer();
    for(i = 0; i < SPEAKER_FEC_SHARE_BUFFER_COUNT; i++) {
        bt_sink_srv_report_id("[FEC]speaker_fec_clear_share_buffer, share_addr[i]:0x%08x", 2, i, share_buffer_addr);
        uint32_t *share_buffer_header = (uint32_t *)share_buffer_addr;
        *share_buffer_header = 0;
        share_buffer_addr += SPEAKER_FEC_SHARE_BUFFER_LENGTH;
    }
}

extern void bt_driver_register_fec_callback(void (*fec_callback)(void *fec_data));
extern bt_status_t bt_avm_set_fec_share_buffer(void *fec_buffer, uint32_t length);
void speaker_fec_init()
{
#ifdef SPEAKER_DUMP_RECOVER_DATA
    g_fec_tag = 0;
#endif
    speaker_fec_init_context();
    bt_avm_set_fec_share_buffer((void *)speaker_fec_get_share_buffer(), SHARE_BUFFER_FEC_SIZE);
    bt_driver_register_fec_callback(speaker_fec_data_callback);
}

void speaker_fec_init_context(void)
{
    speaker_fec_clear_share_buffer();
    bt_sink_srv_memset(&g_speaker_fec_cntx, 0x00, sizeof(g_speaker_fec_cntx));
}

