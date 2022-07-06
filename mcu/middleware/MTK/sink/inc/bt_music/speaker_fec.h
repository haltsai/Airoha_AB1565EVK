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

#include "bt_sink_srv_utils.h"


#define SPEAKER_FEC_CCNI_RECEIVED_VALID_MEDIA_DATA  0x0001
#define SPEAKER_FEC_CCNI_RECEVIED_FEC_DATA          0x0002
#define SPEAKER_FEC_CCNI_RECEVIED_LOST_MEDIA_DATA   0x0003
typedef uint16_t speaker_fec_ccni_recevied_data_type_t;

#define SPEAKER_FEC_CCNI_TO_CONTROLLER_FEC_READY    0x0001
#define SPEAKER_FEC_CCNI_TO_CONTROLLER_RECOVER_DATA 0x0002
typedef uint16_t speaker_fec_to_controller_data_type_t;

#define SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT    4
#define SPEAKER_FEC_MAX_RECEVIED_RAW_DATA_COUNT     8

#define SPEAKER_FEC_GEN_FEC_DATA_COUNT              5

#define SPEAKER_FEC_SEQUENCE_NUMBER_GROUP_BASE_OFFSET     16
#define SPEAKER_FEC_SEQUENCE_NUMBER_GROUP_COUNT_OFFSET    18
#define SPEAKER_FEC_POLY_NOMBER_OFFSET    19

#define SPEAKER_FEC_SHARE_BUFFER_COUNT      10

#define SPEAKER_FEC_PCB_STATE_USED (0x01)
#define SPEAKER_FEC_PCB_STATE_SKIP (0x04)

typedef struct {
    uint16_t sequence_number;
    speaker_fec_ccni_recevied_data_type_t packet_flag;
    uint8_t *data;
    uint32_t data_size;
}speaker_fec_media_data_t;

typedef struct {
    speaker_fec_ccni_recevied_data_type_t data_type;
    uint16_t seq_or_len;
    uint32_t data_addr;
}speaker_fec_ccni_received_data_t;

typedef struct {
    uint16_t gen_fec_count;
    uint16_t start_seq;
    speaker_fec_ccni_received_data_t fec_gen_data[SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT];
}speaker_fec_gen_fec_group_t;

typedef struct {
    uint8_t data_count;
    speaker_fec_gen_fec_group_t fec_gen_group;
    speaker_fec_media_data_t media_data[SPEAKER_FEC_MAX_RECEVIED_RAW_DATA_COUNT];
}speaker_fec_contex_t;

typedef struct {
    uint8_t lost_data_count;
    uint8_t lost_data_index_list[SPEAKER_FEC_AGENT_MAX_FEC_RAW_DATA_COUNT];
}speaker_fec_lost_data_t;

typedef struct {
    uint16_t base_sn;
    uint8_t sn_count;
    uint8_t poly_num;
}speaker_fec_group_info_t;

void speaker_fec_init();
void speaker_fec_init_context(void);
void speaker_fec_set_fec_gen_count(uint8_t fec_count);

