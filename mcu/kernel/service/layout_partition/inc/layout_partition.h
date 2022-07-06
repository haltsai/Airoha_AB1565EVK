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

#ifndef __LAYOUT_PARTITION_H__
#define __LAYOUT_PARTITION_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_LAYOUT_PARTITION_ENABLE

#include <stdint.h>
#include <stdbool.h>

typedef enum{
    PARTITION_SECURITY_HEADER                           = 0x00000000,
    PARTITION_BL                                        = 0x00000001,
    PARTITION_N9                                        = 0x00000002,
    PARTITION_MCU                                       = 0x00000003,
    PARTITION_DSP0                                      = 0x00000004,
    PARTITION_DSP1                                      = 0x00000005,
    PARTITION_FOTA                                      = 0x00000006,
    PARTITION_NVDM                                      = 0x00000007,
    PARTITION_SECURITY_HEADER2                          = 0x00000008,
    PARTITION_ROFS                                      = 0x00000009,
    PARTITION_LM                                        = 0x0000000A,
    PARTITION_LM_GVA                                    = 0x0000000A,
    PARTITION_LM_AMA                                    = 0x0000000B,
    PARTITION_SECURE_FW                                 = 0x0000000C,
    PARTITION_FATFS                                     = 0x0000000D,
    PARTITION_GNSS_CFG                                  = 0x0000000E,

    /* For user customization, partition enumerate please start from 0x80. */
    PARTITION_DUMMY_END                                 = 0x4D4D5544,
    /* It is not allowed to add new data after DUMMY_END
     * and use the partition behind PARTITION_DUMMY_END.
     */
} partition_t;


typedef enum{
    LAYOUT_PARTITION_OK                                 = 0,
    LAYOUT_PARTITION_ERROR                              = -1,
    LAYOUT_PARTITION_NOT_INIT                           = -2,
    LAYOUT_PARTITION_NOT_EXIST                          = -3,
    LAYOUT_PARTITION_NO_MORE_ITEM                       = -4,
    LAYOUT_PARTITION_REPEAT_ID                          = -5,
    LAYOUT_PARTITION_INVALID_PARAMETER                  = -6,
} lp_status;


/* traverse each partition by index example code:
 * uint32_t count;
 * lp_status status;
 * status = lp_get_numbers_of_partition(&count);
 * if(status != LAYOUT_PARTITION_OK){
 *     // error handing
 * }
 *
 * partition_t id;
 * for(uint32_t index = 0; index < count; index++){
 *     status = lp_get_partition_id_by_index(&id, index);
 *     if(status != LAYOUT_PARTITION_OK){
 *         // error handing
 *     }
 *     // lp_begin_address_and_length or something else
 * }
 */


/* traverse each partition by partition id example code:
 * partition_t id;
 * status = lp_get_first_partition_id(&id);
 * if(status != LAYOUT_PARTITION_OK){
 *     // error handing
 * }
 * while(lp_get_next_partition_id(&id) != LAYOUT_PARTITION_NO_MORE_ITEM){
 *     // lp_begin_address_and_length or something else
 * }
 */

/* From the perspective of maintaining forward compatibility,
 * the following two functions are implemented.
 */
uint32_t lp_get_length(partition_t id);
uint32_t lp_get_begin_address(partition_t id);

lp_status lp_init(void);

lp_status lp_get_numbers_of_partition(uint32_t *numbers);
lp_status lp_get_partition_id_by_index(partition_t *id, uint32_t index);

lp_status lp_get_first_partition_id(partition_t *id);
lp_status lp_get_next_partition_id(partition_t *id);

lp_status lp_register_readonly_partitions(const partition_t *id_array, const uint32_t array_len);
lp_status lp_is_readonly(uint32_t address, uint32_t length, bool *is_readonly);

lp_status lp_get_begin_address_and_length(partition_t id, uint32_t *address, uint32_t *length);


#ifdef __cplusplus
}
#endif

#endif /* MTK_LAYOUT_PARTITION_ENABLE */

#endif /* __LAYOUT_PARTITION_H__ */
