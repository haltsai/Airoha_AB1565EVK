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

#ifdef MTK_LAYOUT_PARTITION_ENABLE

#include <assert.h>
#include <stdlib.h>

#ifdef __EXT_BOOTLOADER__
    #include "bl_common.h"
#else
    #include "syslog.h"

    log_create_module(LayoutPartition, PRINT_LEVEL_INFO);
    #define LAYOUT_PARTITION_USE_MSGID_SEND_LOG

    #ifdef LAYOUT_PARTITION_USE_MSGID_SEND_LOG
        #define LP_LOG_I(_message, _arg_cnt, ...)    LOG_MSGID_I(LayoutPartition, _message, _arg_cnt, ##__VA_ARGS__)
        #define LP_LOG_D(_message, _arg_cnt, ...)    LOG_MSGID_D(LayoutPartition, _message, _arg_cnt, ##__VA_ARGS__)
        #define LP_LOG_W(_message, _arg_cnt, ...)    LOG_MSGID_W(LayoutPartition, _message, _arg_cnt, ##__VA_ARGS__)
        #define LP_LOG_E(_message, _arg_cnt, ...)    LOG_MSGID_E(LayoutPartition, _message, _arg_cnt, ##__VA_ARGS__)
    #else
        #define LP_LOG_I(_message, _arg_cnt, ...)    LOG_I(LayoutPartition, _message, ##__VA_ARGS__)
        #define LP_LOG_D(_message, _arg_cnt, ...)    LOG_D(LayoutPartition, _message, ##__VA_ARGS__)
        #define LP_LOG_W(_message, _arg_cnt, ...)    LOG_W(LayoutPartition, _message, ##__VA_ARGS__)
        #define LP_LOG_E(_message, _arg_cnt, ...)    LOG_E(LayoutPartition, _message, ##__VA_ARGS__)
    #endif
#endif

#include "layout_partition.h"
#include "memory_attribute.h"


#ifndef NULL
    #ifdef __cplusplus
        #define NULL                           (0) /*!< NULL */
    #else
        #define NULL                           ((void *)(0)) /*!< NULL */
    #endif
#endif


#define PARTITION_TABLE_ADDR                   ((uint32_t)0x08000000)
#define PARTITION_TABLE_LENGTH                 ((uint32_t)(0x1000))    /* 4KB */

#define PARTITION_TABLE                        ((const partition_item_t *)PARTITION_TABLE_ADDR)
#define PARTITION_ITEM_SIZE                    ((uint32_t)(sizeof(partition_item_t)))
#define PARTITION_ITEM_MAX_NUM                 (PARTITION_TABLE_LENGTH/PARTITION_ITEM_SIZE)
#define PARTITION_TABLE_END_ADDR               (PARTITION_TABLE_ADDR + PARTITION_ITEM_SIZE*PARTITION_ITEM_MAX_NUM)

/* Use special characters to indicate the end of the scan. */
#define PARTITION_DUMMY_END_VALUE              PARTITION_DUMMY_END    /* "DUMM" */
#define DUMMY_END_VALUE                        0x444E4559             /* "YEND" */

typedef struct {
    uint32_t id;
    uint32_t attribute;
    uint32_t load_address_high;
    uint32_t load_address_low;

    uint32_t length_high;
    uint32_t length_low;
    uint32_t execution_address;
    uint32_t customized_data[5];
} partition_item_t;

typedef struct{
    partition_t id;
    uint32_t address;
} id_address_t;

typedef struct{
    bool initialized;

    uint32_t count;
    id_address_t *array;

    partition_t *readonly;
    uint32_t ro_array_len;
} lp_control_block_t, LPCB;

ATTR_ZIDATA_IN_TCM static LPCB lpm;
/* no malloc, using static array */
#ifdef __EXT_BOOTLOADER__
ATTR_ZIDATA_IN_TCM static id_address_t array[PARTITION_ITEM_MAX_NUM];
#endif


static void print_all_partition_information(void);
static lp_status find_partition_item_address(partition_t target_id, uint32_t *addr, uint32_t *idx);

lp_status lp_init(void)
{
    const partition_item_t *ptr = NULL;
    uint32_t addr = 0, temp_idx = 0, inner_idx = 0;
    id_address_t swap;

    lpm.initialized = false;
    lpm.count = 0;
    lpm.array = NULL;
    lpm.readonly = NULL;
    lpm.ro_array_len = NULL;

    for(addr = PARTITION_TABLE_ADDR; addr < PARTITION_TABLE_END_ADDR; addr += PARTITION_ITEM_SIZE){
        ptr = (const partition_item_t *)addr;
        if(((*(volatile uint32_t *)(addr)) == (uint32_t)PARTITION_DUMMY_END) &&
           ((*(volatile uint32_t *)(addr + 4U)) == (uint32_t)DUMMY_END_VALUE)
        ){
            break;
        } else {
#ifdef __EXT_BOOTLOADER__
            bl_print(LOG_DEBUG, "id:%x, address:%x\r\n", ptr->id, addr);
#else
            LP_LOG_I("id:%08x, address:%08x\r\n", 2, ptr->id, addr);
#endif
            ++lpm.count;
        }
    }

#ifdef __EXT_BOOTLOADER__
    lpm.array = array;
#else
    assert(lpm.count);
    lpm.array = (id_address_t *)malloc(sizeof(id_address_t)*lpm.count);
#endif

    for(addr = PARTITION_TABLE_ADDR; addr < PARTITION_TABLE_END_ADDR; addr += PARTITION_ITEM_SIZE){
        ptr = (const partition_item_t *)addr;
        lpm.array[temp_idx].id = (partition_t)(ptr->id);
        lpm.array[temp_idx].address = addr;
        ++temp_idx;
        if(temp_idx >= lpm.count){
            break;
        }
    }

    /* sort the lpm.array by id descending */
    for(temp_idx = 0; temp_idx < (lpm.count - 1); temp_idx++){
        for(inner_idx = 0; inner_idx < (lpm.count - 1 - temp_idx); inner_idx++){
            if(lpm.array[inner_idx].id > lpm.array[inner_idx+1].id){
                swap.id = lpm.array[inner_idx].id;
                swap.address = lpm.array[inner_idx].address;

                lpm.array[inner_idx].id = lpm.array[inner_idx+1].id;
                lpm.array[inner_idx].address = lpm.array[inner_idx+1].address;

                lpm.array[inner_idx+1].id = swap.id;
                lpm.array[inner_idx+1].address = swap.address;
            } else if(lpm.array[inner_idx].id == lpm.array[inner_idx+1].id){
                lpm.initialized = false;
#ifdef __EXT_BOOTLOADER__
                bl_print(LOG_ERROR, "there has repeat id !!!\r\n");
#else
                LP_LOG_E("there has repeat id !!!\r\n", 0);
#endif
                return LAYOUT_PARTITION_REPEAT_ID;
            } else{
                ;
            }
        }
    }

    print_all_partition_information();
    lpm.initialized = true;

    return LAYOUT_PARTITION_OK;
}


uint32_t lp_get_begin_address(partition_t id)
{
    uint32_t partition_item_addr, index;
    lp_status status;
    status = find_partition_item_address(id, &partition_item_addr, &index);
    index = index; /* avoid build warning */
    assert(status == LAYOUT_PARTITION_OK);
    const partition_item_t *ptr = (const partition_item_t *)partition_item_addr;
    return ptr->load_address_low;
}


uint32_t lp_get_length(partition_t id)
{
    uint32_t partition_item_addr, index;
    lp_status status;
    status = find_partition_item_address(id, &partition_item_addr, &index);
    index = index; /* avoid build warning */
    assert(status == LAYOUT_PARTITION_OK);
    const partition_item_t *ptr = (const partition_item_t *)partition_item_addr;
    return ptr->length_low;
}


lp_status lp_get_begin_address_and_length(partition_t id, uint32_t *address, uint32_t *length)
{
    uint32_t partition_item_addr, index;
    lp_status status;
    if((address == NULL) || (length == NULL)){
        return LAYOUT_PARTITION_INVALID_PARAMETER;
    }
    status = find_partition_item_address(id, &partition_item_addr, &index);
    if(status != LAYOUT_PARTITION_OK){
        return status;
    }
    index = index; /* avoid build warning */
    const partition_item_t *ptr = (const partition_item_t *)partition_item_addr;

    *address = ptr->load_address_low;
    *length = ptr->length_low;
    return LAYOUT_PARTITION_OK;
}


lp_status lp_get_numbers_of_partition(uint32_t *numbers)
{
    if(numbers == NULL){
        return LAYOUT_PARTITION_INVALID_PARAMETER;
    }
    *numbers = lpm.count;
    return LAYOUT_PARTITION_OK;
}


lp_status lp_get_first_partition_id(partition_t *id)
{
    if(lpm.initialized != true){
        return LAYOUT_PARTITION_NOT_INIT;
    } else if(id == NULL){
        return LAYOUT_PARTITION_INVALID_PARAMETER;
    } else if(lpm.count == 0){
        return LAYOUT_PARTITION_NO_MORE_ITEM;
    } else{
        ;
    }
    *id = lpm.array[0].id;
    return LAYOUT_PARTITION_OK;
}


lp_status lp_get_next_partition_id(partition_t *id)
{
    uint32_t partition_item_addr, index;
    lp_status status;
    if(id == NULL){
        return LAYOUT_PARTITION_INVALID_PARAMETER;
    }

    status = find_partition_item_address(*id, &partition_item_addr, &index);
    partition_item_addr = partition_item_addr; /* avoid build warning */
    if(status != LAYOUT_PARTITION_OK){
        return status;
    }
    if(index == (lpm.count - 1)){
        return LAYOUT_PARTITION_NO_MORE_ITEM;
    }

    *id = lpm.array[index+1].id;
    return LAYOUT_PARTITION_OK;
}


lp_status lp_get_partition_id_by_index(partition_t *id, uint32_t index)
{
    if(lpm.initialized == false){
        return LAYOUT_PARTITION_NOT_INIT;
    }
    if((index > (lpm.count - 1)) || (id == NULL)){
        return LAYOUT_PARTITION_INVALID_PARAMETER;
    }

    uint32_t partition_item_addr = PARTITION_TABLE_ADDR + index*PARTITION_ITEM_SIZE;
    const partition_item_t *ptr = (const partition_item_t *)partition_item_addr;
    *id = ptr->id;
    return LAYOUT_PARTITION_OK;
}


lp_status lp_register_readonly_partitions(const partition_t *id_array, const uint32_t array_len)
{
    if((id_array == NULL) || (array_len == 0)){
        return LAYOUT_PARTITION_INVALID_PARAMETER;
    }
    if(lpm.initialized == false){
        return LAYOUT_PARTITION_NOT_INIT;
    }
    /* Ensure that the input partition IDs exist in the partition table. */
    uint32_t idx, inner_idx, array_idx = 0xFFFFFFFF;
    for(idx = 0; idx < array_len; idx++){
        for(inner_idx = 0; inner_idx < lpm.count; inner_idx++){
            if(id_array[idx] == lpm.array[inner_idx].id){
                array_idx = idx;
                break;
            }
        }
        if(array_idx == 0xFFFFFFFF){
            return LAYOUT_PARTITION_NOT_EXIST;
        }
    }

    lpm.readonly = id_array;
    lpm.ro_array_len = array_len;
    return LAYOUT_PARTITION_OK;
}


lp_status lp_is_readonly(uint32_t address, uint32_t length, bool *is_readonly)
{
    uint32_t start, end, idx, partition_start, partition_end, partition_len;
    lp_status status = LAYOUT_PARTITION_OK;
    if(address > ((uint32_t)0xFFFFFFFF - length)){
        return LAYOUT_PARTITION_INVALID_PARAMETER;
    }
    start = address;    end = address + length;
    *is_readonly = false;
    for(idx = 0; idx < lpm.ro_array_len; idx++){
        lp_get_begin_address_and_length(lpm.readonly[idx], &partition_start, &partition_len);
        partition_end = partition_start + partition_len;
        if( ((start >= partition_start) && (start < partition_end)) ||
            ((end > partition_start) && (end < partition_end))
        ){
            *is_readonly = true;
#ifdef __EXT_BOOTLOADER__
            bl_print(LOG_WARN, "{%x, %x} in {%x, %x}\r\n", start, end, partition_start, partition_end);
#else
            LP_LOG_E("{%08x, %08x} in {%08x, %08x}\r\n", 4, start, end, partition_start, partition_end);
#endif
            break;
        }
    }
    return LAYOUT_PARTITION_OK;
}


static void print_all_partition_information(void)
{

    uint32_t idx;
    const partition_item_t* ptr = NULL;
#ifdef __EXT_BOOTLOADER__
    bl_print(LOG_WARN, "id address load_address length\r\n");
#else
    LP_LOG_E("id address load_address length\r\n", 0);
#endif
    for(idx = 0; idx < lpm.count; idx++){
        ptr = (const partition_item_t *)(lpm.array[idx].address);
#ifdef __EXT_BOOTLOADER__
        bl_print(LOG_WARN, "%x %x %x %x\r\n", lpm.array[idx].id, lpm.array[idx].address, ptr->load_address_low, ptr->length_low);
#else
        LP_LOG_E("%08x %08x %08x %08x\r\n", 4, lpm.array[idx].id, lpm.array[idx].address, ptr->load_address_low, ptr->length_low);
#endif
    }
}


static lp_status find_partition_item_address(partition_t target_id, uint32_t *addr, uint32_t *idx)
{
    uint32_t tmp_address = 0xFFFFFFFF;
    uint32_t start = 0, end = lpm.count - 1, mid;
    partition_t id;
    lp_status status = LAYOUT_PARTITION_OK;

    while(start <= end){
        mid = start + ((end - start)>>1U);
        id = lpm.array[mid].id;
        if(id == target_id){
            *addr = lpm.array[mid].address;
            *idx = mid;
            tmp_address = lpm.array[mid].address;
            break;
        } else if(id > target_id){
            end = mid - 1;
        } else{
            start = mid + 1;
        }
    }


    if(tmp_address == 0xFFFFFFFF){
#ifdef __EXT_BOOTLOADER__
        bl_print(LOG_CRIT, "find_partition_item_address failed with id %x !!!\r\n", target_id);
#else
        LP_LOG_E("find_partition_item_address failed with id %x !!!\r\n", 1, target_id);
#endif
        return LAYOUT_PARTITION_NOT_EXIST;
    }

    return status;
}

#endif /* MTK_LAYOUT_PARTITION_ENABLE */