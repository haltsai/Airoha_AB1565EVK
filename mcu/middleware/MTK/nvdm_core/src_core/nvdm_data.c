/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#ifdef MTK_NVDM_ENABLE

#include "nvdm.h"
#include "nvdm_port.h"

#ifdef __EXT_BOOTLOADER__
#include "nvdm_config.h"
#endif

#include "nvdm_internal.h"
#include "nvdm_msgid_log.h"

int32_t g_sum_data_item_headers = 0;
data_item_header_t *g_data_item_headers;
int32_t *g_gc_buffer;
uint32_t g_max_data_item_size;
uint32_t g_max_group_name_size;
uint32_t g_max_data_item_name_size;
uint32_t g_total_data_item_count;
uint8_t g_working_buffer[NVDM_BUFFER_SIZE];
uint8_t g_migration_buffer[NVDM_BUFFER_SIZE];

#ifdef SYSTEM_DAEMON_TASK_ENABLE
nvdm_dcll_t g_nbw_reqs;

/* In order to make nvdm_write_data_item aware that it is called
 * by nvdm_write_data_item_non_blocking, this flag be added.
 */
bool g_write_req_from_nbw;
async_nw_para_t *g_curr_write;
#endif

extern bool nvdm_init_status;
extern uint32_t g_nvdm_peb_size;
extern peb_info_t *g_pebs_info;

static void data_item_header_print_info(data_item_header_t *header)
{
    nvdm_log_msgid_info(nvdm_001, 0);
    nvdm_log_msgid_info(nvdm_002, 1, header->status);
    nvdm_log_msgid_info(nvdm_003, 1, header->pnum);
    nvdm_log_msgid_info(nvdm_004, 1, header->offset);
    nvdm_log_msgid_info(nvdm_005, 1, header->sequence_number);
    nvdm_log_msgid_info(nvdm_006, 1, header->group_name_size);
    nvdm_log_msgid_info(nvdm_007, 1, header->data_item_name_size);
    nvdm_log_msgid_info(nvdm_008, 1, header->value_size);
    nvdm_log_msgid_info(nvdm_009, 1, header->index);
    nvdm_log_msgid_info(nvdm_010, 1, header->type);
    nvdm_log_msgid_info(nvdm_011, 1, header->hash_name);
}

#ifndef __EXT_BOOTLOADER__
static int32_t find_empty_data_item(void)
{
    int32_t i;

    for (i = 0; i < g_total_data_item_count; i++) {
        if (g_data_item_headers[i].value_size == 0) {
            return i;
        }
    }

    return -1;
}
#endif /* #ifndef __EXT_BOOTLOADER__ */

static uint16_t calculate_checksum(uint16_t checksum, const uint8_t *buffer, int32_t size)
{
    uint8_t *byte_checksum;
    int32_t i;

    byte_checksum = (uint8_t *)&checksum;

    for (i = 0; i < size; i++) {
        if (i & 0x01) {
            *(byte_checksum + 1) += *(buffer + i);
        } else {
            *byte_checksum += *(buffer + i);
        }
    }

    return checksum;
}

static uint16_t calculate_data_item_checksum(data_item_header_t *header, int32_t pnum, int32_t position)
{
    int32_t i, offset, fragment;
    uint16_t checksum;
    uint8_t *working_buffer = g_working_buffer;

    checksum = 0;

    /* checksum for data item's header
        * skip frist byte because it's not calculated by checksum.
        */
    checksum = calculate_checksum(checksum, &header->pnum, DATA_ITEM_HEADER_SIZE - 1);

    /* add checksum for group name and data item name */
    offset = position;
    peb_read_data(pnum, offset, working_buffer, header->group_name_size + header->data_item_name_size);
    checksum = calculate_checksum(checksum, working_buffer, header->group_name_size + header->data_item_name_size);

    /* add checksum for data item's value */
    offset += header->group_name_size + header->data_item_name_size;
    fragment = header->value_size / NVDM_BUFFER_SIZE;
    for (i = 0; i < fragment; i++) {
        memset(working_buffer, 0, NVDM_BUFFER_SIZE);
        peb_read_data(pnum, offset, working_buffer, NVDM_BUFFER_SIZE);
        checksum = calculate_checksum(checksum, working_buffer, NVDM_BUFFER_SIZE);
        offset += NVDM_BUFFER_SIZE;
    }
    if (header->value_size % NVDM_BUFFER_SIZE) {
        memset(working_buffer, 0, NVDM_BUFFER_SIZE);
        peb_read_data(pnum, offset, working_buffer, header->value_size % NVDM_BUFFER_SIZE);
        checksum = calculate_checksum(checksum, working_buffer, header->value_size % NVDM_BUFFER_SIZE);
    }

    return checksum;
}

static int32_t search_data_item_by_name(const char *group_name, const char *data_item_name, uint32_t *hasename)
{
    int32_t i, len = 0;
    uint32_t hash, a = 63689, b = 378551;
    char str[64];

    for (i = 0; * (group_name + i) != '\0'; i++) {
        str[i] = *(group_name + i);
    }
    len += i;
    for (i = 0; * (data_item_name + i) != '\0'; i++) {
        str[i + len] = *(data_item_name + i);
    }
    len += i;
    str[len] = '\0';

    hash = 0;
    for (i = 0; i < len; i++) {
        hash = hash * a + str[i];
        a = a * b;
    }

    if (hasename != NULL) {
        *hasename = hash;
    }
    nvdm_log_msgid_info(nvdm_012, 1, hash);

    for (i = 0; i < g_total_data_item_count; i++) {
        if (g_data_item_headers[i].value_size == 0) {
            continue;
        }
        if (hash == g_data_item_headers[i].hash_name) {
            peb_read_data(g_data_item_headers[i].pnum, g_data_item_headers[i].offset + DATA_ITEM_HEADER_SIZE, (uint8_t *)str, g_data_item_headers[i].group_name_size);
            if (strcmp(str, group_name)) {
                continue;
            }
            peb_read_data(g_data_item_headers[i].pnum, g_data_item_headers[i].offset + DATA_ITEM_HEADER_SIZE + g_data_item_headers[i].group_name_size, (uint8_t *)str, g_data_item_headers[i].data_item_name_size);
            if (strcmp(str, data_item_name) == 0) {
                return i;
            }
        }
    }

    return -1;
}


#ifdef SYSTEM_DAEMON_TASK_ENABLE
nvdm_status_t nvdm_read_data_from_nbw_reqs(
    const char *group_name, const char *data_item_name,
    uint8_t *buffer, uint32_t *size)
{
    uint32_t grp_name_size = 0, item_name_size = 0;
    nvdm_status_t op_res = NVDM_STATUS_ITEM_NOT_FOUND;
    async_nw_para_t *p_nw_para = (async_nw_para_t *)(g_nbw_reqs.prev);
    char *p_grp_name, *p_item_name;
    uint8_t *p_buf, *p_data;

    if(p_nw_para == (async_nw_para_t *)(&g_nbw_reqs)){
        op_res = NVDM_STATUS_ITEM_NOT_FOUND;
    } else {
        /* Traverse all the data on the double circular linked list in reverse order,
         * and compare the group name and item name.
         */
        grp_name_size = strlen(group_name)+1;
        item_name_size = strlen(data_item_name)+1;
        while(p_nw_para != (async_nw_para_t *)(&g_nbw_reqs)){
            p_buf = (uint8_t *)p_nw_para;
            p_buf += sizeof(async_nw_para_t);
            p_grp_name = (char *)p_buf;
            p_buf += p_nw_para->group_name_size;
            p_item_name = (char *)p_buf;
            p_buf += p_nw_para->data_item_name_size;
            p_data = p_buf;

            if((grp_name_size == p_nw_para->group_name_size) &&
               (item_name_size == p_nw_para->data_item_name_size) &&
               (strncmp(group_name, p_grp_name, grp_name_size) == 0) &&
               (strncmp(data_item_name, p_item_name, item_name_size) == 0)
            ){
                if(p_nw_para->data_item_size <= *size){
                    memcpy(buffer, p_data, p_nw_para->data_item_size);
                    buffer += p_nw_para->data_item_size;
                    memset(buffer, 0, *size - p_nw_para->data_item_size);
                    *size = p_nw_para->data_item_size;
                    op_res = NVDM_STATUS_OK;
                    break;
                } else {
                    op_res = NVDM_STATUS_INVALID_PARAMETER;
                    *size = 0;
                }
            }

            p_nw_para = (async_nw_para_t *)(p_nw_para->item.prev);
        }
    }

    return op_res;
}
#endif

nvdm_status_t nvdm_read_data_item(const char *group_name,
                                  const char *data_item_name,
                                  uint8_t *buffer,
                                  uint32_t *size)
{
    int32_t index;
    uint32_t offset;
    uint16_t checksum1, checksum2;
#ifdef SYSTEM_DAEMON_TASK_ENABLE
    nvdm_status_t read_from_ram = NVDM_STATUS_OK;
#endif

    nvdm_log_msgid_info(nvdm_013, 0);

    if ((group_name == NULL) ||
            (data_item_name == NULL) ||
            (buffer == NULL) ||
            (size == NULL) ||
            (*size == 0)) {
        if (size != NULL) {
            *size = 0;
        }
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if ((strlen(group_name) > g_max_group_name_size) ||
            (strlen(data_item_name) > g_max_data_item_name_size)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_protect_mutex_take();

#ifdef SYSTEM_DAEMON_TASK_ENABLE
    if(nvdm_port_query_task_handler() == false){
        /* For the consistency of API behavior, if the current task does not
         * start to query NVDM data, then first try to read the data from RAM,
         * and then read from Flash.
         * Otherwise, only read data from Flash.
         */
        read_from_ram = nvdm_read_data_from_nbw_reqs(group_name, data_item_name, buffer, size);
        if(read_from_ram != NVDM_STATUS_ITEM_NOT_FOUND){
            nvdm_port_protect_mutex_give();
            return read_from_ram;
        }
    }
#endif

    index = search_data_item_by_name(group_name, data_item_name, NULL);
    if (index < 0) {
        nvdm_port_protect_mutex_give();
        *size = 0;
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    /* check whether buffer size is enough */
    if (*size < g_data_item_headers[index].value_size) {
        *size = 0;
        nvdm_port_protect_mutex_give();
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    /* verify checksum of date item */
    checksum1 = calculate_data_item_checksum(&g_data_item_headers[index],
                g_data_item_headers[index].pnum,
                g_data_item_headers[index].offset + DATA_ITEM_HEADER_SIZE);
    offset = g_data_item_headers[index].offset + DATA_ITEM_HEADER_SIZE + g_data_item_headers[index].group_name_size + g_data_item_headers[index].data_item_name_size + g_data_item_headers[index].value_size;
    peb_read_data(g_data_item_headers[index].pnum, offset, (uint8_t *)&checksum2, DATA_ITEM_CHECKSUM_SIZE);
    if (checksum1 != checksum2) {
        *size = 0;

        if ( g_data_item_headers[index].value_size == 0 ){
            /* before calc checksum of read_item, 
             * someone delete the same item by modifing value_size variable 
             */
            nvdm_port_protect_mutex_give();
            return NVDM_STATUS_ITEM_NOT_FOUND;
        }
        nvdm_port_protect_mutex_give();
        return NVDM_STATUS_INCORRECT_CHECKSUM;
    }

    /* checksum is ok, so read it to user buffer */
    offset = g_data_item_headers[index].offset + DATA_ITEM_HEADER_SIZE + g_data_item_headers[index].group_name_size + g_data_item_headers[index].data_item_name_size;
    peb_read_data(g_data_item_headers[index].pnum, offset, buffer, g_data_item_headers[index].value_size);
    buffer += g_data_item_headers[index].value_size;
    memset(buffer, 0, *size - g_data_item_headers[index].value_size);

    *size = g_data_item_headers[index].value_size;

    nvdm_log_info("group_name = %s, data_item_name = %s, size = %d", group_name, data_item_name, g_data_item_headers[index].value_size);

    nvdm_port_protect_mutex_give();

    return NVDM_STATUS_OK;
}

#ifndef __EXT_BOOTLOADER__
static nvdm_status_t write_nvdm_parameter_check(const char *group_name,
                                                           const char *data_item_name,
                                                           nvdm_data_item_type_t type,
                                                           const uint8_t *buffer,
                                                           uint32_t size)
{
    if ((group_name == NULL) ||
            (data_item_name == NULL) ||
            (buffer == NULL) ||
            (size > g_max_data_item_size) ||
            (size == 0)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if ((type != NVDM_DATA_ITEM_TYPE_RAW_DATA) &&
            (type != NVDM_DATA_ITEM_TYPE_STRING)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if ((strlen(group_name) > g_max_group_name_size) ||
            (strlen(data_item_name) > g_max_data_item_name_size)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    return NVDM_STATUS_OK;
}


static int32_t cmp_item_data_in_buffer_and_flash(const uint8_t *buffer, const data_item_header_t *header)
{
    /* Compare whether the data on the RAM is the same as the data on the flash. */
    int32_t i, offset, fragment, cmp_res = 0;
    uint8_t *working_buffer = g_working_buffer;
    uint32_t cmp_len;

    /* Calculate the offset where the user data is located. */
    offset = header->offset + sizeof(data_item_header_t) + header->group_name_size + header->data_item_name_size;

    fragment = header->value_size / NVDM_BUFFER_SIZE;
    for (i = 0; i < fragment; i++) {
        peb_read_data(header->pnum, offset, working_buffer, NVDM_BUFFER_SIZE);
        offset += NVDM_BUFFER_SIZE;
        cmp_len = NVDM_BUFFER_SIZE;
        cmp_res = memcmp(working_buffer, buffer, cmp_len);
        if(cmp_res != 0){
            return cmp_res;
        }
        buffer += cmp_len;
    }
    if (header->value_size % NVDM_BUFFER_SIZE) {
        peb_read_data(header->pnum, offset, working_buffer, header->value_size % NVDM_BUFFER_SIZE);
        cmp_len = header->value_size % NVDM_BUFFER_SIZE;
        cmp_res = memcmp(working_buffer, buffer, cmp_len);
        if(cmp_res != 0){
            return cmp_res;
        }
        buffer += cmp_len;
    }

    return cmp_res;
}


nvdm_status_t nvdm_write_data_item(const char *group_name,
                                   const char *data_item_name,
                                   nvdm_data_item_type_t type,
                                   const uint8_t *buffer,
                                   uint32_t size)
{
    int32_t append, peb_status_update;
    int32_t added_size, alloc_size, group_name_size, data_item_name_size;
    int32_t index, pnum, offset;
    uint32_t hashname;
    uint32_t checksum_twice_match = 1;
    uint16_t checksum, checksum_after_write;
    uint8_t *working_buffer;
    data_item_header_t *p_data_item_header;
    data_item_header_t *p_old_item_header;
    data_item_header_t data_item_header;
    data_item_status_t status;
    int32_t ret, cmp_res = 0;
    nvdm_status_t nvdm_status;

    nvdm_log_msgid_info(nvdm_015, 0);

    nvdm_status = write_nvdm_parameter_check(group_name, data_item_name, type, buffer, size);
    if (nvdm_status != NVDM_STATUS_OK) {
        return nvdm_status;
    }

    nvdm_port_mutex_take();

#ifdef SYSTEM_DAEMON_TASK_ENABLE
    /* Since at the application layer, both blocking and non-blocking writes to
     * the same data may be called, the Driver needs to be aware of this situation.
     * In this case, NVDM driver will make the blocked write data take effect,
     * and delete requests for non-blocking writes at the same time.
     */
    nvdm_cancel_non_blocking_write(group_name, data_item_name);
#endif

    group_name_size = strlen(group_name) + 1;
    data_item_name_size = strlen(data_item_name) + 1;

    index = search_data_item_by_name(group_name, data_item_name, &hashname);
    nvdm_log_msgid_info(nvdm_016, 1, index);
    if (index < 0) {
        append = 1;
        /* find a empty position to fill in */
        index = find_empty_data_item();
    } else {
        append = 0;
        if(g_data_item_headers[index].value_size == size){
            /*
             * When writing an existing data item, it is necessary to compare
             * whether the user data matches the existing data on the flash.
             * If the data matches, there is no need to write flash, just return OK.
             */
            nvdm_port_protect_mutex_take();
            cmp_res = cmp_item_data_in_buffer_and_flash(buffer, &(g_data_item_headers[index]));
            if(cmp_res == 0){
                nvdm_port_protect_mutex_give();
                nvdm_port_mutex_give();
                return NVDM_STATUS_OK;
            }
            nvdm_port_protect_mutex_give();
        }
    }

    /* check whether we have enough free space for append */
    if (append) {
        added_size = size + group_name_size + data_item_name_size + DATA_ITEM_HEADER_SIZE + DATA_ITEM_CHECKSUM_SIZE;
    } else {
        added_size = (int32_t)size - (int32_t)g_data_item_headers[index].value_size;
    }
    ret = space_is_enough(added_size);
    if (ret == false) {
        nvdm_log_msgid_info(nvdm_017, 0);
        nvdm_port_mutex_give();
        return NVDM_STATUS_INSUFFICIENT_SPACE;
    }

    /* find a peb with require free space to place data item */
    alloc_size = size + group_name_size + data_item_name_size + DATA_ITEM_HEADER_SIZE + DATA_ITEM_CHECKSUM_SIZE;
    pnum = space_allocation(alloc_size, added_size, &offset, group_name, data_item_name);

    /* write item header information to temp header first */
    p_data_item_header = &data_item_header;
    p_old_item_header = &(g_data_item_headers[index]);
    if (append) {
        if (g_sum_data_item_headers >= g_total_data_item_count) {
            nvdm_log_msgid_warning(nvdm_018, 0);
            nvdm_port_mutex_give();
            return NVDM_STATUS_ERROR;
        }
        /* new add a NVDM entry */
        nvdm_log_msgid_info(nvdm_019, 0);
        p_data_item_header->type = type;
        p_data_item_header->hash_name = hashname;
        p_data_item_header->value_size = size;
        p_data_item_header->reserved = 0xFF00 | (index >> 8);
        p_data_item_header->index = index & 0xFF;
        g_sum_data_item_headers++;

        /* first write */
        p_data_item_header->sequence_number = 1;
        p_data_item_header->pnum = pnum;
        p_data_item_header->group_name_size = group_name_size;
        p_data_item_header->data_item_name_size = data_item_name_size;
        p_data_item_header->offset = offset;
    } else {
        /* modify an old NVDM entry */
        nvdm_log_msgid_info(nvdm_020, 0);
        memcpy(p_data_item_header, p_old_item_header, sizeof(data_item_header_t));

        /* Use sequence_number to record the number of writes of data items. */
        p_data_item_header->sequence_number = (p_old_item_header->sequence_number) + 1;
        p_data_item_header->pnum = pnum;
        p_data_item_header->offset = offset;
        p_data_item_header->value_size = size;
        /* old version use only one byte to decide the index */
        if (p_data_item_header->reserved == 0x00FF) {
            p_data_item_header->reserved = 0xFF00;
        }
    }
    p_data_item_header->status = DATA_ITEM_STATUS_WRITING;

    nvdm_log_warning("task: %s write { %s, %s } with %d bytes", nvdm_port_get_curr_task_name(), group_name, data_item_name, size);

    /* calculate checksum for new data item copy */
    checksum = 0;
    /* DATA_ITEM_HEADER_SIZE-1 must be power of 2 */
    checksum = calculate_checksum(checksum, &p_data_item_header->pnum, DATA_ITEM_HEADER_SIZE - 1);

    /* can't use g_working_buffer because of the race condition with nvdm_read_data_item */
    working_buffer = nvdm_port_malloc((group_name_size + data_item_name_size));
    if (working_buffer == NULL){
        nvdm_port_mutex_give();
        return NVDM_STATUS_ERROR;
    }

    memcpy(working_buffer, group_name, group_name_size);
    working_buffer += group_name_size;
    memcpy(working_buffer, data_item_name, data_item_name_size);
    working_buffer -= group_name_size;
    checksum = calculate_checksum(checksum, working_buffer, (group_name_size + data_item_name_size));
    checksum = calculate_checksum(checksum, buffer, size);

    /* this peb is frist written, so status of PEB need to be modified */
    if (peb_activing(pnum)) {
        peb_status_update = 1;
    } else {
        peb_status_update = 0;
    }

    peb_sub_free(pnum, alloc_size);

    /* set status of data item to writing */
    status = DATA_ITEM_STATUS_WRITING;
    peb_write_data(pnum, offset, (uint8_t *)&status, 1);
    p_data_item_header->status = DATA_ITEM_STATUS_WRITING;
    nvdm_port_poweroff(1);
    /* write header of data item (not including status) */
    offset += 1;
    peb_write_data(pnum, offset, &p_data_item_header->pnum, DATA_ITEM_HEADER_SIZE - 1);
    /* write group name and data item name */
    offset += DATA_ITEM_HEADER_SIZE - 1;
    peb_write_data(pnum, offset, working_buffer, group_name_size + data_item_name_size);

    /* this variable will not be used again */
    nvdm_port_free(working_buffer);

    /* write value of data item */
    offset += group_name_size + data_item_name_size;
    peb_write_data(pnum, offset, (uint8_t *)buffer, size);
    /* write checksum of data item */
    offset += size;
    peb_write_data(pnum, offset, (uint8_t *)&checksum, DATA_ITEM_CHECKSUM_SIZE);

    nvdm_port_protect_mutex_take();
    /* It shares the global buffer g_working_buffer with nvdm read operation
     * and needs to be mutually exclusive with read behavior.
     * For compatibility reasons, the global physical address is not used directly to access the flash.
     */
    checksum_after_write = calculate_data_item_checksum(
                               p_data_item_header,
                               p_data_item_header->pnum,
                               p_data_item_header->offset + DATA_ITEM_HEADER_SIZE);
    if(checksum != checksum_after_write){
        nvdm_log_warning("task %s write { %s, %s }, calc checksum twice: 0x%08X and 0x%08X, return -2",
            nvdm_port_get_curr_task_name(),
            group_name, data_item_name,
            checksum, checksum_after_write
        );
        checksum_twice_match = 0;
    }
    nvdm_port_protect_mutex_give();

    /* set status of data item to valid */
    offset -= p_data_item_header->value_size + DATA_ITEM_HEADER_SIZE + group_name_size + data_item_name_size;

    /* If before and after writing, the checksum calculated using user input is inconsistent,
     * indicating that the buffer is not thread-safe !!!
     */
    if(checksum_twice_match == 0){
        status = DATA_ITEM_STATUS_DELETE;
    } else {
        status = DATA_ITEM_STATUS_VALID;
    }
    peb_write_data(pnum, offset, (uint8_t *)&status, 1);
    if(checksum_twice_match == 0){
        p_data_item_header->status = DATA_ITEM_STATUS_DELETE;
    } else {
        p_data_item_header->status = DATA_ITEM_STATUS_VALID;
    }

    nvdm_port_poweroff(2);

    if (peb_status_update) {
        /* now we have at least one data item in PEB,
              * so update it's status to PEB_STATUS_ACTIVED
              */
        peb_update_status(pnum, PEB_STATUS_ACTIVED);
        nvdm_port_poweroff(3);
    }

    if ((!append) && (checksum_twice_match == 1)) {
        /* because we have write new copy successfully,
              * so we can invalidate old copy now!
              */
        status = DATA_ITEM_STATUS_DELETE;
        peb_write_data(p_old_item_header->pnum, p_old_item_header->offset, (uint8_t *)&status, 1);
        nvdm_port_poweroff(4);
        /* mark drity for old copy */
        size = DATA_ITEM_HEADER_SIZE +
               p_old_item_header->group_name_size +
               p_old_item_header->data_item_name_size +
               p_old_item_header->value_size +
               DATA_ITEM_CHECKSUM_SIZE;
        peb_add_drity(p_old_item_header->pnum, size);
    }

    /* after user data had write to flash, update the header information to global variable */
    nvdm_port_protect_mutex_take();
    if(checksum_twice_match == 1){
        memcpy(p_old_item_header, p_data_item_header, sizeof(data_item_header_t));

        /* Modify g_valid_data_size after the data is actually written. */
        space_sub_valid(-added_size);
    }
    nvdm_port_protect_mutex_give();

    nvdm_port_mutex_give();

    if(checksum_twice_match == 1){
        return NVDM_STATUS_OK;
    } else {
        return NVDM_STATUS_INCORRECT_CHECKSUM;
    }
}

#ifdef SYSTEM_DAEMON_TASK_ENABLE


bool nbw_req_is_empty(void)
{
    async_nw_para_t *nbw_para;
    nvdm_port_protect_mutex_take();
    nbw_para = (async_nw_para_t *)(g_nbw_reqs.next);
    if(nbw_para != (async_nw_para_t *)(&g_nbw_reqs)){
        nvdm_port_protect_mutex_give();
        return false;
    }
    nvdm_port_protect_mutex_give();
    return true;
}


static bool same_item_with_last_nbw_req(const char *group_name, const char *data_item_name, uint32_t new_item_size)
{
    async_nw_para_t *last_elem;
    last_elem = (async_nw_para_t *)(g_nbw_reqs.prev);
    bool ret = false;
    uint32_t gn_size, din_size;
    const char *p_le_gn, *p_le_din;       /* pointers of last element group name and data item name */

    gn_size = strlen(group_name)+1;       /* group name size */
    din_size = strlen(data_item_name)+1;  /* data item name size */

    p_le_gn = (const char*)((uint8_t*)last_elem + sizeof(async_nw_para_t));
    p_le_din = (const char*)((uint8_t*)p_le_gn + last_elem->group_name_size);

    if((gn_size == last_elem->group_name_size) &&
       (din_size == last_elem->data_item_name_size) &&
       (new_item_size == last_elem->data_item_size) &&
       (strcmp(p_le_gn, group_name) == 0) &&
       (strcmp(p_le_din, data_item_name) == 0)
    ){
        /* Only when the group name, data item name,
         * and actual data length are all the same,
         * it is considered the same data.
         */
        ret = true;
    }

    return ret;
}


static async_nw_para_t * construct_nbw_req(const char *group_name,
                                    const char *data_item_name,
                                    nvdm_data_item_type_t type,
                                    const uint8_t *buffer,
                                    uint32_t size,
                                    const nvdm_user_callback_t callback,
                                    const void *user_data)
{
    uint32_t group_name_size, data_item_name_size, alloc_size;
    uint8_t *p_buf;
    async_nw_para_t *nbw_para = NULL;

    group_name_size = strlen(group_name) + 1;
    data_item_name_size = strlen(data_item_name) + 1;
    alloc_size = sizeof(async_nw_para_t) + group_name_size + data_item_name_size + size;
    nbw_para = (async_nw_para_t *)nvdm_port_malloc(alloc_size);
    if (nbw_para == NULL) {
        nvdm_log_msgid_error(nvdm_023, 0);
        /* Ignore Malloc fail and just return Error code. */
        return nbw_para;
    }

    nbw_para->item.prev = NULL;
    nbw_para->item.next = NULL;
    nbw_para->callback = callback;
    nbw_para->user_data = (void *)user_data;
    nbw_para->group_name_size = group_name_size;
    nbw_para->data_item_name_size = data_item_name_size;
    nbw_para->data_item_size = size;
    nbw_para->type = type;
    p_buf = (uint8_t *)nbw_para + sizeof(async_nw_para_t);
    memcpy(p_buf, group_name, group_name_size);
    p_buf += group_name_size;
    memcpy(p_buf, data_item_name, data_item_name_size);
    p_buf += data_item_name_size;
    memcpy(p_buf, buffer, size);
    return nbw_para;
}


nvdm_status_t nvdm_write_data_item_non_blocking(const char *group_name,
                                   const char *data_item_name,
                                   nvdm_data_item_type_t type,
                                   const uint8_t *buffer,
                                   uint32_t size,
                                   const nvdm_user_callback_t callback,
                                   const void *user_data)
{
    uint8_t *p_buf;
    async_nw_para_t *nbw_para;
    nvdm_status_t status;

    nvdm_log_msgid_info(nvdm_022, 0);

    if (callback == NULL) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }
    status = write_nvdm_parameter_check(group_name, data_item_name, type, buffer, size);
    if (status != NVDM_STATUS_OK) {
        return status;
    }

    nvdm_port_protect_mutex_take();
    if(nbw_req_is_empty() == true){
        /* If the double-linked list is empty, construct nbw_req directly and add it to the linked list. */
        nbw_para = construct_nbw_req(group_name, data_item_name, type, buffer, size, callback, user_data);
        if(nbw_para == NULL){
            nvdm_port_protect_mutex_give();
            return NVDM_STATUS_ERROR;
        }
        if(nvdm_port_send_queue() == false){
            nvdm_port_free(nbw_para);
            nvdm_log_msgid_warning(nvdm_024, 0);
            nvdm_port_protect_mutex_give();
            return NVDM_STATUS_ERROR;
        } else {
            /* Make sure that there is no non-blocking write requirement and
             * send the request to the daemon task before appending the data on the NVDM linked list.
             */
            /* Insert data from the end of the doubly circular linked list. */
            nbw_para->item.next = (&g_nbw_reqs);
            nbw_para->item.prev = (g_nbw_reqs.prev);
            g_nbw_reqs.prev->next = (nvdm_dcll_t *)nbw_para;
            g_nbw_reqs.prev = (nvdm_dcll_t *)nbw_para;
        }
    } else {
        /* If the linked list is not empty, it means that a non-blocking write event
         * has been sent to the daemon task before, so there is no need to send it again here.
         */

        if((same_item_with_last_nbw_req(group_name, data_item_name, size) == true) &&
           ((async_nw_para_t *)(g_nbw_reqs.prev) != g_curr_write)
        ){
            /* If the last element has the same group name and item name
             * as the element user want to write again,
             * then the two pieces of data can be merged directly on RAM.
             * The data can be merged if and only if the last item
             * is not in the writing state and has the same name.
             */
            nbw_para = (async_nw_para_t *)(g_nbw_reqs.prev);
            nbw_para->callback(NVDM_STATUS_ITEM_MERGED_BY_NBW, nbw_para->user_data);
            p_buf = (uint8_t*)nbw_para + sizeof(async_nw_para_t) + \
                    nbw_para->group_name_size + nbw_para->data_item_name_size;
            memcpy(p_buf, buffer, size);
            nbw_para->callback = (nvdm_user_callback_t)callback;
            nbw_para->user_data = (void *)user_data;
        } else {
            nbw_para = construct_nbw_req(group_name, data_item_name, type, buffer, size, callback, user_data);
            /* Insert data from the end of the doubly circular linked list. */
            nbw_para->item.next = (&g_nbw_reqs);
            nbw_para->item.prev = (g_nbw_reqs.prev);
            g_nbw_reqs.prev->next = (nvdm_dcll_t *)nbw_para;
            g_nbw_reqs.prev = (nvdm_dcll_t *)nbw_para;
        }
    }
    nvdm_port_protect_mutex_give();

    return NVDM_STATUS_OK;
}

void system_daemon_nvdm_msg_handler(void)
{
    async_nw_para_t *nbw_para;
    char *group_name;
    char *data_item_name;
    uint8_t *p_data;
    nvdm_data_item_type_t type;
    uint8_t *p_buf;
    nvdm_status_t status;
    bool no_nbw_req = false;

    nvdm_port_protect_mutex_take();
    no_nbw_req = nbw_req_is_empty();
    if(no_nbw_req == false){
        /* Mark the API that calls blocking writes as non-blocking writes,
         * otherwise the data may be deleted.
         */
        g_write_req_from_nbw = true;
        nbw_para = (async_nw_para_t *)(g_nbw_reqs.next);
        g_curr_write = nbw_para;
    }
    nvdm_port_protect_mutex_give();

    if(no_nbw_req == false){
        p_buf = (uint8_t *)nbw_para;
        p_buf += sizeof(async_nw_para_t);
        group_name = (char *)p_buf;
        p_buf += nbw_para->group_name_size;
        data_item_name = (char *)p_buf;
        p_buf += nbw_para->data_item_name_size;
        p_data = p_buf;
        type = nbw_para->type;

        /* Because the mutex used for reading and writing is separated,
         * if driver use protect mutex to protect nvdm_write_data_item, it may cause !!! DEADLOCK !!!.
         * At the same time, protect mutex is not used here,
         * so a new Non-Blocking Write request is allowed during data writing.
         */
        status = nvdm_write_data_item(group_name, data_item_name, type, p_data, nbw_para->data_item_size);
        nbw_para->callback(status, nbw_para->user_data);

        nvdm_port_protect_mutex_take();
        /* !!! NOTE !!!
         * The relationship of nbw_para on the double-linked list may change,
         * but it does not affect the removal of this item from the DCLL.
         */

        /* Remove the specified data item from the list. */
        nbw_para->item.prev->next = nbw_para->item.next;
        nbw_para->item.next->prev = nbw_para->item.prev;
        nvdm_port_free((void *)nbw_para);

        if(nbw_req_is_empty() == false){
            /* If there is still data on RAM that has not been written to Flash,
             * then the NVDM non-blocking write event is sent back to the daemon task.
             */
            nvdm_port_send_queue();
        }
        g_write_req_from_nbw = false;
        g_curr_write = NULL;
        nvdm_port_protect_mutex_give();
    }
}


nvdm_status_t nvdm_cancel_non_blocking_write(const char* group_name, const char* item_name)
{
    nvdm_status_t op_res = NVDM_STATUS_ITEM_NOT_FOUND;
    async_nw_para_t *nbw_para, *backup_next;
    uint8_t *p_buf;
    char *p_nbw_grp_name, *p_nbw_item_name;
    uint8_t cur_grp_name_size, cur_item_name_size;
    uint32_t canceled_item_count = 0;

    cur_grp_name_size = strlen(group_name) + 1;
    cur_item_name_size = strlen(item_name) + 1;

    if((group_name == NULL) ||
       (item_name == NULL) ||
       (cur_grp_name_size == 1) ||
       (cur_item_name_size == 1)
    ){
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    nvdm_port_protect_mutex_take();

    nbw_para = ((async_nw_para_t *)(g_nbw_reqs.next));
    while(nbw_para != ((async_nw_para_t *)(&g_nbw_reqs))){
        if(nbw_para == NULL){
            nvdm_port_must_assert();
        }
        p_buf = (uint8_t *)nbw_para;
        p_buf += sizeof(async_nw_para_t);
        p_nbw_grp_name = (char *)p_buf;
        p_buf += nbw_para->group_name_size;
        p_nbw_item_name = (char *)p_buf;
        p_buf += nbw_para->data_item_name_size;

        nvdm_log_info("nvdm_cancel_nbw { %s, %s } travers { %s, %s }",
            group_name, item_name, p_nbw_grp_name, p_nbw_item_name);

        backup_next = (async_nw_para_t *)(nbw_para->item.next);

        if((nbw_para->group_name_size == cur_grp_name_size) &&
            (nbw_para->data_item_name_size == cur_item_name_size)
        ){
            if((strcmp(group_name, p_nbw_grp_name) == 0) &&
                (strcmp(item_name, p_nbw_item_name) == 0)
            ){
                if((g_write_req_from_nbw == true) &&
                   (nbw_para == ((async_nw_para_t *)(g_nbw_reqs.next)))
                ){
                    /* NVDM driver will call this API when using blocking writing method,
                     * but in this case, it cannot be cancelled,
                     * otherwise the non-blocking write will be lost.
                     */
                    op_res = NVDM_STATUS_ERROR;
                    break;
                } else {
                    /* If the driver does not cancel by itself, or
                     * the data being written is not the first in the DCLL,
                     * then the non-blocking write data can be cancelled.
                     */

                    nbw_para->callback(NVDM_STATUS_ITEM_MERGED_BY_BW, nbw_para->user_data);

                    /* Remove the specified data item from the list. */
                    nbw_para->item.prev->next = nbw_para->item.next;
                    nbw_para->item.next->prev = nbw_para->item.prev;
                    nvdm_port_free((void *)nbw_para);

                    nvdm_log_error("The User of { %s, %s } uses both BW & NBW.", group_name, item_name);
                    op_res = NVDM_STATUS_OK;

                    /* There may be multiple NBW requests to the same item on DCLL,
                     * so driver can't break out of the loop directly here.
                     */
                    ++canceled_item_count;
                }
            }
        }

        nbw_para = backup_next;
    }
    if(canceled_item_count != 0){
        nvdm_log_msgid_warning(nvdm_117, 1, canceled_item_count);
    }
    nvdm_port_protect_mutex_give();
    return op_res;
}

#endif

static void data_item_delete(uint32_t index)
{
    uint32_t size;
    data_item_status_t status;

    /* change the status of data item in flash from valid to delete */
    status = DATA_ITEM_STATUS_DELETE;
    peb_write_data(g_data_item_headers[index].pnum,
                   g_data_item_headers[index].offset,
                   (uint8_t *)&status,
                   1);
    nvdm_port_poweroff(2);
    /* recalculate the dirty value of that PEB */
    size = DATA_ITEM_HEADER_SIZE +
           g_data_item_headers[index].group_name_size +
           g_data_item_headers[index].data_item_name_size +
           g_data_item_headers[index].value_size +
           DATA_ITEM_CHECKSUM_SIZE;
    peb_add_drity(g_data_item_headers[index].pnum, size);
    /* free the data item header in memory */
    g_data_item_headers[index].value_size = 0;
    /* update global variables */
    g_sum_data_item_headers--;
    space_sub_valid(size);
}

nvdm_status_t nvdm_delete_data_item(const char *group_name, const char *data_item_name)
{
    int32_t index;

    nvdm_log_msgid_info(nvdm_025, 0);

    if ((group_name == NULL) || (data_item_name == NULL)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }
    if ((strlen(group_name) > g_max_group_name_size) ||
            (strlen(data_item_name) > g_max_data_item_name_size)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_mutex_take();

    index = search_data_item_by_name(group_name, data_item_name, NULL);
    if (index < 0) {
        nvdm_port_mutex_give();
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    nvdm_log_warning("task: %s delete { %s, %s }", nvdm_port_get_curr_task_name(), group_name, data_item_name);
    data_item_delete(index);

    nvdm_port_mutex_give();

    return NVDM_STATUS_OK;
}

nvdm_status_t nvdm_delete_group(const char *group_name)
{
    uint32_t index;
    char str[64];
    bool delete_done;

    nvdm_log_msgid_info(nvdm_026, 0);

    if (group_name == NULL) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }
    if (strlen(group_name) > g_max_group_name_size) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_mutex_take();

    delete_done = false;
    for (index = 0; index < g_total_data_item_count; index++) {
        /* skip free date item header */
        if (g_data_item_headers[index].value_size == 0) {
            continue;
        }
        /* check whether group name is match */
        peb_read_data(g_data_item_headers[index].pnum, g_data_item_headers[index].offset + DATA_ITEM_HEADER_SIZE, (uint8_t *)str, g_data_item_headers[index].group_name_size);
        if (strcmp(str, group_name) != 0) {
            continue;
        }
        data_item_delete(index);
        delete_done = true;
    }

    nvdm_port_mutex_give();

    if (delete_done == false) {
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    return NVDM_STATUS_OK;
}

nvdm_status_t nvdm_delete_all(void)
{
    uint32_t index;
    bool delete_done;

    nvdm_log_msgid_info(nvdm_027, 0);

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_mutex_take();

    delete_done = false;
    for (index = 0; index < g_total_data_item_count; index++) {
        /* skip free date item header */
        if (g_data_item_headers[index].value_size == 0) {
            continue;
        }
        data_item_delete(index);
        delete_done = true;
    }

    nvdm_port_mutex_give();

    if (delete_done == false) {
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    return NVDM_STATUS_OK;
}

static uint32_t g_group_name_count, g_curr_group_name_index;
static uint32_t g_curr_data_item_name_index;
static uint32_t *g_order_data_item_name_table;
static uint32_t *g_group_name_table;
static bool g_nvdm_query_is_active = false;

nvdm_status_t nvdm_query_begin(void)
{
    bool is_frist_group_name;
    uint32_t i, malloc_size, curr_order_index;
    uint8_t *search_bitmask;
    char search_str[64], compare_str[64];

    nvdm_log_msgid_info(nvdm_028, 0);

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_mutex_take();

    if (g_nvdm_query_is_active == true) {
        nvdm_port_mutex_give();
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_get_task_handler();

    /* malloc memory used by search table */
    malloc_size = (2 * g_sum_data_item_headers + 1) * sizeof(uint32_t) + (g_total_data_item_count / 8 + 1);
    g_group_name_table = nvdm_port_malloc(malloc_size);
    if (g_group_name_table == NULL) {
        nvdm_port_mutex_give();
        return NVDM_STATUS_ERROR;
    }
    memset(g_group_name_table, 0, malloc_size);
    g_order_data_item_name_table = g_group_name_table + g_sum_data_item_headers + 1; /* need one more */
    search_bitmask = (uint8_t *)(g_order_data_item_name_table + g_sum_data_item_headers);

    /* search and reorder the group name table */
    curr_order_index = 0;
    g_group_name_count = 0;
    while (curr_order_index < g_sum_data_item_headers) {
        /* search next group name */
        is_frist_group_name = true;
        for (i = 0; i < g_total_data_item_count; i++) {
            /* skip free hole and the data item searched before */
            if ((g_data_item_headers[i].value_size == 0) ||
                    (search_bitmask[i / 8] & (1 << (i % 8)))) {
                continue;
            }
            /* the frist data item is dirrectly used as search name */
            if (is_frist_group_name == true) {
                g_group_name_table[g_group_name_count++] = curr_order_index;
                peb_read_data(g_data_item_headers[i].pnum, g_data_item_headers[i].offset + DATA_ITEM_HEADER_SIZE, (uint8_t *)search_str, g_data_item_headers[i].group_name_size);
                g_order_data_item_name_table[curr_order_index++] = i;
                search_bitmask[i / 8] |= 1 << (i % 8);
                is_frist_group_name = false;
            } else {
                peb_read_data(g_data_item_headers[i].pnum, g_data_item_headers[i].offset + DATA_ITEM_HEADER_SIZE, (uint8_t *)compare_str, g_data_item_headers[i].group_name_size);
                if (strcmp(search_str, compare_str) == 0) {
                    g_order_data_item_name_table[curr_order_index++] = i;
                    search_bitmask[i / 8] |= 1 << (i % 8);
                }
            }
        }
    }
    g_group_name_table[g_group_name_count] = g_sum_data_item_headers;
    g_curr_group_name_index = 0;

    g_nvdm_query_is_active = true;

    return NVDM_STATUS_OK;
}

nvdm_status_t nvdm_query_end(void)
{
    nvdm_log_msgid_info(nvdm_029, 0);

    if (nvdm_port_query_task_handler() == false) {
        return NVDM_STATUS_ERROR;
    }

    if (g_nvdm_query_is_active == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_free(g_group_name_table);

    g_nvdm_query_is_active = false;
    nvdm_port_reset_task_handler();

    nvdm_port_mutex_give();

    return NVDM_STATUS_OK;
}

nvdm_status_t nvdm_query_next_group_name(char *group_name)
{
    uint32_t index;
    data_item_header_t *data_item_headers;

    nvdm_log_msgid_info(nvdm_030, 0);

    if (group_name == NULL) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_port_query_task_handler() == false) {
        return NVDM_STATUS_ERROR;
    }

    if (g_nvdm_query_is_active == false) {
        return NVDM_STATUS_ERROR;
    }

    if (g_curr_group_name_index >= g_group_name_count) {
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    index = g_group_name_table[g_curr_group_name_index++];
    index = g_order_data_item_name_table[index];
    data_item_headers = &g_data_item_headers[index];
    /* check whether group name is match */
    peb_read_data(data_item_headers->pnum, data_item_headers->offset + DATA_ITEM_HEADER_SIZE, (uint8_t *)group_name, data_item_headers->group_name_size);

    g_curr_data_item_name_index = 0;

    return NVDM_STATUS_OK;
}

nvdm_status_t nvdm_query_data_item_count(const char *group_name, uint32_t *count)
{
    if ((group_name == NULL) || (count == NULL)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_port_query_task_handler() == false) {
        return NVDM_STATUS_ERROR;
    }

    if (g_nvdm_query_is_active == false) {
        return NVDM_STATUS_ERROR;
    }

    /* Forbid to call nvdm_query_next_data_item_name() before nvdm_query_next_group_name() */
    if (g_curr_group_name_index == 0) {
        return NVDM_STATUS_ERROR;
    }

    *count = g_group_name_table[g_curr_group_name_index] - g_group_name_table[g_curr_group_name_index - 1];

    return NVDM_STATUS_OK;
}

nvdm_status_t nvdm_query_next_data_item_name(char *data_item_name)
{
    uint32_t index;
    data_item_header_t *data_item_headers;

    nvdm_log_msgid_info(nvdm_031, 0);

    if (data_item_name == NULL) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_port_query_task_handler() == false) {
        return NVDM_STATUS_ERROR;
    }

    if (g_nvdm_query_is_active == false) {
        return NVDM_STATUS_ERROR;
    }

    /* Forbid to call nvdm_query_next_data_item_name() before nvdm_query_next_group_name() */
    if (g_curr_group_name_index == 0) {
        return NVDM_STATUS_ERROR;
    }

    if (g_curr_data_item_name_index >= (g_group_name_table[g_curr_group_name_index] - g_group_name_table[g_curr_group_name_index - 1])) {
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    index = g_group_name_table[g_curr_group_name_index - 1];
    index = g_order_data_item_name_table[index + g_curr_data_item_name_index++];
    data_item_headers = &g_data_item_headers[index];
    /* check whether group name is match */
    peb_read_data(data_item_headers->pnum,
                  data_item_headers->offset + DATA_ITEM_HEADER_SIZE + data_item_headers->group_name_size,
                  (uint8_t *)data_item_name,
                  data_item_headers->data_item_name_size);

    return NVDM_STATUS_OK;
}

nvdm_status_t nvdm_query_data_item_length(const char *group_name, const char *data_item_name, uint32_t *size)
{
    int32_t index;

    nvdm_log_msgid_info(nvdm_032, 0);

    if ((group_name == NULL) ||
            (data_item_name == NULL) ||
            (size == NULL)) {
        if (size != NULL) {
            *size = 0;
        }
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if ((strlen(group_name) > g_max_group_name_size) ||
            (strlen(data_item_name) > g_max_data_item_name_size)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_protect_mutex_take();

    index = search_data_item_by_name(group_name, data_item_name, NULL);
    if (index < 0) {
        nvdm_port_protect_mutex_give();
        *size = 0;
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    *size = g_data_item_headers[index].value_size;

    nvdm_port_protect_mutex_give();

    nvdm_log_info("group_name = %s, data_item_name = %s, size = %d", group_name, data_item_name, *size);

    return NVDM_STATUS_OK;
}


nvdm_status_t nvdm_query_data_item_type(const char *group_name, const char *data_item_name, nvdm_data_item_type_t *type)
{
    int32_t index;

    if ((group_name == NULL) ||
            (data_item_name == NULL) ||
            (type == NULL)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if ((strlen(group_name) > g_max_group_name_size) ||
            (strlen(data_item_name) > g_max_data_item_name_size)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_protect_mutex_take();

    index = search_data_item_by_name(group_name, data_item_name, NULL);
    if (index < 0) {
        nvdm_port_protect_mutex_give();
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    *type = g_data_item_headers[index].type;

    nvdm_port_protect_mutex_give();

    return NVDM_STATUS_OK;
}

#endif /* #ifndef __EXT_BOOTLOADER__ */

static bool calculate_data_item_index(data_item_header_t *data_item_header, uint32_t *curr_index)
{
    bool is_old_index;

    if (data_item_header->reserved == 0xFF) {
        *curr_index = data_item_header->index;
        is_old_index = true;
    } else {
        *curr_index = (uint32_t)(data_item_header->index) + ((uint32_t)(data_item_header->reserved & 0xFF) << 8);
        is_old_index = false;
    }

    return is_old_index;
}

void data_item_scan(int32_t pnum)
{
    uint32_t curr_index;
    int32_t offset, oldpnum, old_offset;
    int32_t peb_drity, peb_valid;
    data_item_header_t data_item_header;
    uint16_t checksum1, checksum2;
    uint32_t size;
    static int32_t abnormal_data_item = -1;
    data_item_status_t status;

    const char *p_group_name = NULL, *p_item_name = NULL;
    uint32_t scan_limit = g_nvdm_peb_size - PEB_HEADER_SIZE - DATA_ITEM_HEADER_SIZE;

    nvdm_log_msgid_info(nvdm_034, 1, pnum);
    offset = 0;
    peb_drity = 0;
    peb_valid = 0;
    /* scan entire peb content */
    while (offset < scan_limit) {
        peb_read_data(pnum, offset, (uint8_t *)&data_item_header, DATA_ITEM_HEADER_SIZE);
        data_item_header_print_info(&data_item_header);
        size = DATA_ITEM_HEADER_SIZE + data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size + DATA_ITEM_CHECKSUM_SIZE;
        if((size > scan_limit) ||
           (peb_drity + size > g_nvdm_peb_size) ||
           (peb_valid + size > g_nvdm_peb_size)
        ){
            /* When the size of a data item exceeds the specified range, it may cause data confusion.
             * Here, the subsequent data of the flash block is marked as garbage, and it will be processed during garbage collection later.
             */
            if(data_item_header.status != DATA_ITEM_STATUS_EMPTY){
                nvdm_log_msgid_error(nvdm_116, 5, pnum, offset, size, peb_valid, peb_drity);
                peb_add_drity(pnum, (g_nvdm_peb_size - PEB_HEADER_SIZE) - peb_drity - peb_valid);
                return;
            }
        }
        switch (data_item_header.status) {
            case DATA_ITEM_STATUS_EMPTY:
                peb_add_free(pnum, (g_nvdm_peb_size - PEB_HEADER_SIZE) - peb_drity - peb_valid);
                return;
            case DATA_ITEM_STATUS_WRITING:
                /* we can't belive data item header if we found it's writting,
                          * so just mark rest of space is dirty.
                          */
                peb_add_drity(pnum, (g_nvdm_peb_size - PEB_HEADER_SIZE) - peb_drity - peb_valid);
                return;
            case DATA_ITEM_STATUS_VALID:
                break;
            case DATA_ITEM_STATUS_DELETE:
                peb_drity += size;
                offset += size;
                peb_add_drity(pnum, size);
                continue;
            default:
                /* Mark the data behind it as garbage, so that the data will be reclaimed during GC. */
                nvdm_log_msgid_error(nvdm_035, 2, pnum, offset);
                peb_add_drity(pnum, (g_nvdm_peb_size - PEB_HEADER_SIZE) - peb_drity - peb_valid);
                return;
        }

        calculate_data_item_index(&data_item_header, &curr_index);
        if (curr_index >= g_total_data_item_count) {
            nvdm_log_msgid_error(nvdm_036, 2, g_total_data_item_count, curr_index);
            /* Must assert the configuration error, or else NVDM can not work. */
            nvdm_port_must_assert();
            return;
        }

        old_offset = offset;

        offset += DATA_ITEM_HEADER_SIZE + data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size;
        peb_read_data(pnum, offset, (uint8_t *)&checksum2, DATA_ITEM_CHECKSUM_SIZE);
        offset += DATA_ITEM_CHECKSUM_SIZE;

        /* verify checksum of data item */
        if(data_item_header.group_name_size + data_item_header.data_item_name_size > NVDM_BUFFER_SIZE){
            /* skip the invalid data item header with wrong name size */
            checksum1 = ~checksum2;
        } else {
            checksum1 = calculate_data_item_checksum(&data_item_header, pnum, data_item_header.offset + DATA_ITEM_HEADER_SIZE);
        }
        if (checksum1 != checksum2) {
            nvdm_log_msgid_warning(nvdm_037, 0);

            peb_drity += size;
            peb_add_drity(pnum, size);

            /* When the checksum error is detected, the item needs to be deleted.
             * If it is not deleted, always assert may occur !!!
             * Since the checksum of the current item is incorrect, its index may be occupied by other items.
             * If NVDM need to recycle the item whose checksum is abnormal when garbage collection is triggered,
             * the driver will determine that there is a pair of items with the same index, and their status are valid.
             * This situation is absolutely not allowed in NVDM.
             */
            data_item_header.status = DATA_ITEM_STATUS_DELETE;
            peb_write_data(pnum, old_offset, (const uint8_t *)(&(data_item_header.status)), 1);

            /* For compatibility reasons, the global physical address is not used directly to access the flash.
             * Here the address is only offset against the Flash starting address.
             */
            if(data_item_header.group_name_size + data_item_header.data_item_name_size <= NVDM_BUFFER_SIZE){
                p_group_name = (const char *)(&g_working_buffer);
                peb_read_data(
                    pnum,
                    old_offset + DATA_ITEM_HEADER_SIZE,
                    (uint8_t *)p_group_name,
                    data_item_header.group_name_size
                );
                g_working_buffer[data_item_header.group_name_size] = '\0';

                p_item_name = (const char *)(&g_working_buffer + data_item_header.group_name_size);
                peb_read_data(
                    pnum,
                    old_offset + DATA_ITEM_HEADER_SIZE + data_item_header.group_name_size,
                    (uint8_t *)p_item_name,
                    data_item_header.data_item_name_size
                );
                g_working_buffer[data_item_header.group_name_size + data_item_header.data_item_name_size] = '\0';

                nvdm_log_warning("mark { %s, %s }, ( %d, %d ) <==> ( %d, %d ) with read size %d as DELETE",
                    p_group_name, p_item_name, pnum, old_offset,
                    data_item_header.pnum, data_item_header.offset, size
                );
            }

            continue;
        }

        /* update count of data items */
        if (g_data_item_headers[curr_index].sequence_number == MAX_WRITE_SEQUENCE_NUMBER) {
            /* we find this frist time */
            memcpy(&g_data_item_headers[curr_index], &data_item_header, sizeof(data_item_header_t));
            g_sum_data_item_headers++;
            if (g_sum_data_item_headers > g_total_data_item_count) {
                nvdm_log_msgid_error(nvdm_038, 0);
                /* Must assert the configuration error, or else NVDM can not work. */
                nvdm_port_must_assert();
            }
            peb_valid += size;
        } else {
            /* we found it before, so compare sequence number of them
                    * this is possible that new copy is total update
                    * but old copy has not been invalidated when power-off happen.
                    */
            nvdm_log_msgid_info(nvdm_039, 0);
            nvdm_log_msgid_warning(nvdm_040, 4,
                          g_data_item_headers[curr_index].pnum,
                          g_data_item_headers[curr_index].offset,
                          data_item_header.pnum,
                          data_item_header.offset);
            if (abnormal_data_item > 0) {
                /* this should only happen once at most */

                /* The item has been checked by the checksum.
                 * But there are two or more pairs of NVDM data items with the same Index( NVDM internal ID ).
                 * Delete data items that were discovered later.
                 */
                nvdm_log_msgid_error(nvdm_041, 3, abnormal_data_item, pnum, old_offset);
                status = DATA_ITEM_STATUS_DELETE;
                peb_write_data(pnum, old_offset, (uint8_t *)&status, 1);
                peb_drity += size;
                peb_add_drity(pnum, size);
                ++abnormal_data_item;
                continue;
            } else {
                abnormal_data_item = 1;
            }
            if (g_data_item_headers[curr_index].sequence_number < data_item_header.sequence_number) {
                /* we find new copy, so mark old peb as delete */
                status = DATA_ITEM_STATUS_DELETE;
                peb_write_data(g_data_item_headers[curr_index].pnum,
                               g_data_item_headers[curr_index].offset,
                               (uint8_t *)&status,
                               1);
                nvdm_port_poweroff(5);
                /* add valid info */
                peb_valid += size;
                /* add dirty info */
                oldpnum = g_data_item_headers[curr_index].pnum;
                size = g_data_item_headers[curr_index].value_size +
                       g_data_item_headers[curr_index].group_name_size +
                       g_data_item_headers[curr_index].data_item_name_size +
                       DATA_ITEM_CHECKSUM_SIZE + DATA_ITEM_HEADER_SIZE;
                peb_add_drity(oldpnum, size);
                /* if we found old copy in same peb, we must substract it's size from peb_valid */
                if (oldpnum == pnum) {
                    peb_valid -= size;
                }
                memcpy(&g_data_item_headers[curr_index], &data_item_header, sizeof(data_item_header_t));

                /* if we found it in the same peb last time */
                if (oldpnum == pnum) {
                    peb_drity += size;
                }
            } else {
                /* we find old copy, so mark it as delete directly */
                status = DATA_ITEM_STATUS_DELETE;
                peb_write_data(data_item_header.pnum,
                               data_item_header.offset,
                               (uint8_t *)&status,
                               1);
                nvdm_port_poweroff(6);
                peb_drity += size;
                peb_add_drity(pnum, size);
            }
        }
    }

    /* If there is dark space exist, it should also be considered as free space. */
    if (offset >= (g_nvdm_peb_size - PEB_HEADER_SIZE - DATA_ITEM_HEADER_SIZE)) {
        peb_add_free(pnum, (g_nvdm_peb_size - PEB_HEADER_SIZE) - peb_drity - peb_valid);
    }
}

#ifdef __EXT_BOOTLOADER__
uint8_t g_item_header_buffer[DATA_ITEM_HEADER_SIZE*NVDM_PORT_DAT_ITEM_COUNT];
#endif

nvdm_status_t data_item_init(void)
{
    uint32_t i;

    g_total_data_item_count = nvdm_port_get_data_item_config(&g_max_data_item_size, &g_max_group_name_size, &g_max_data_item_name_size);
    if (g_max_data_item_size > MAX_DATA_ITEM_SIZE) {
        nvdm_log_msgid_error(nvdm_042, 0);
        /* Must assert the configuration error, or else NVDM can not work. */
        nvdm_port_must_assert();
        return NVDM_STATUS_ERROR;
    }

#ifndef __EXT_BOOTLOADER__
    g_data_item_headers = (data_item_header_t *)nvdm_port_malloc(g_total_data_item_count * sizeof(data_item_header_t));
#else
    g_data_item_headers = (data_item_header_t *)g_item_header_buffer;
#endif
    if (g_data_item_headers == NULL) {
        nvdm_log_msgid_error(nvdm_043, 0);
        /* Ignore the scenario where malloc fails and does not trigger assert,
         * but the program needs to ensure that NVDM cannot work.
         */
        return NVDM_STATUS_ERROR;
    }

    memset(g_data_item_headers, 0, g_total_data_item_count * sizeof(data_item_header_t));
    for (i = 0; i < g_total_data_item_count; i++) {
        g_data_item_headers[i].sequence_number = MAX_WRITE_SEQUENCE_NUMBER;
    }
    g_sum_data_item_headers = 0;

    return NVDM_STATUS_OK;
}

static void data_migration(int32_t src_pnum, int32_t src_offset,
                           int32_t dst_pnum, int32_t dst_offset, int32_t size)
{
    int32_t i, delta, fragment;
    uint8_t *working_buffer = g_migration_buffer;

    fragment = size / NVDM_BUFFER_SIZE;
    delta = 0;
    for (i = 0; i < fragment; i++) {
        memset(working_buffer, 0, NVDM_BUFFER_SIZE);
        peb_read_data(src_pnum, src_offset + delta, working_buffer, NVDM_BUFFER_SIZE);
        peb_write_data(dst_pnum, dst_offset + delta, working_buffer, NVDM_BUFFER_SIZE);
        delta += NVDM_BUFFER_SIZE;
    }
    if (size % NVDM_BUFFER_SIZE) {
        memset(working_buffer, 0, NVDM_BUFFER_SIZE);
        peb_read_data(src_pnum, src_offset + delta, working_buffer, size % NVDM_BUFFER_SIZE);
        peb_write_data(dst_pnum, dst_offset + delta, working_buffer, size % NVDM_BUFFER_SIZE);
    }
}

int32_t data_item_migration(int32_t src_pnum, int32_t dst_pnum, int32_t offset)
{
    data_item_header_t data_item_header, tmp_hdr;
    data_item_status_t status;
    int32_t pos, size, scan_range;
    uint16_t checksum;
    uint32_t curr_index;
    bool is_old_index;

    /* search valid data item */
    pos = 0;
    scan_range = g_nvdm_peb_size - PEB_HEADER_SIZE - g_pebs_info[src_pnum].free;
    while (pos < scan_range) {
        peb_read_data(src_pnum, pos, (uint8_t *)&data_item_header, DATA_ITEM_HEADER_SIZE);
        size = DATA_ITEM_HEADER_SIZE +
                data_item_header.group_name_size + data_item_header.data_item_name_size +
                data_item_header.value_size +
                DATA_ITEM_CHECKSUM_SIZE;

        if((size > scan_range) ||
           ((pos + size) > scan_range)
        ){
            peb_print_info();
            nvdm_log_msgid_warning(nvdm_116, 5, src_pnum, pos, size, scan_range, data_item_header.status);
            return offset;
        }

        switch (data_item_header.status) {
            case DATA_ITEM_STATUS_WRITING:
            case DATA_ITEM_STATUS_EMPTY:
                /* no more data item after it, just return */
                return offset;
            case DATA_ITEM_STATUS_DELETE:
                /* do nothing, just skip it to find next data item.
                          * data item is marked as delete status, it must be an old copy.
                          */
                pos += size;
                break;
            case DATA_ITEM_STATUS_VALID:
                is_old_index = calculate_data_item_index(&data_item_header, &curr_index);
                if ((g_data_item_headers[curr_index].pnum != data_item_header.pnum) ||
                    (g_data_item_headers[curr_index].offset != data_item_header.offset)
                ) {
                    /* find old copy, this should not happen,
                                 * because it's fixed in init phase.
                                 */
                    nvdm_log_msgid_error(nvdm_044, 5,
                                   src_pnum, pos,
                                   g_data_item_headers[curr_index].pnum,
                                   g_data_item_headers[curr_index].offset,
                                   size);

                    /* In the process of nvdm_init, delete data items whose status is VALID but with abnormal data.
                     * Delete data items that were discovered later.
                     */
                    status = DATA_ITEM_STATUS_DELETE;
                    peb_write_data(src_pnum, pos, (uint8_t *)&status, 1);
                    /* update offset for next item */
                    pos += size;
                    peb_add_drity(src_pnum, size);
                } else {
                    memcpy(&tmp_hdr, &g_data_item_headers[curr_index], DATA_ITEM_HEADER_SIZE);

                    /* find up-to-date copy, so migrate it to target peb update header */
                    tmp_hdr.pnum = dst_pnum;
                    tmp_hdr.offset = offset;

                    /* old version use only one byte to decide the index */
                    if (is_old_index) {
                        tmp_hdr.reserved = 0xFF00;
                    }

                    /* calculate new checksum */
                    nvdm_port_protect_mutex_take();
                    checksum = calculate_data_item_checksum(&tmp_hdr, src_pnum, data_item_header.offset + DATA_ITEM_HEADER_SIZE);
                    nvdm_port_protect_mutex_give();

                    /* mark writ of beginning */
                    status = DATA_ITEM_STATUS_WRITING;
                    peb_write_data(dst_pnum, offset, (uint8_t *)&status, 1);
                    nvdm_port_poweroff(7);

                    /* write header of data item */
                    peb_write_data(dst_pnum, offset + 1, &tmp_hdr.pnum, DATA_ITEM_HEADER_SIZE - 1);

                    /* write group name, data item name and value of data item */
                    data_migration(src_pnum, pos + DATA_ITEM_HEADER_SIZE, dst_pnum, offset + DATA_ITEM_HEADER_SIZE, data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size);

                    /* write checksum of data item */
                    peb_write_data(dst_pnum, offset + DATA_ITEM_HEADER_SIZE + data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size, (uint8_t *)&checksum, DATA_ITEM_CHECKSUM_SIZE);

                    /* mark write of end */
                    status = DATA_ITEM_STATUS_VALID;
                    peb_write_data(dst_pnum, offset, (uint8_t *)&status, 1);
                    nvdm_port_poweroff(8);

                    /* substract free size of target peb */
                    peb_sub_free(dst_pnum, size);

                    /* update offset for next write */
                    offset += size;
                    pos += size;

                    nvdm_port_protect_mutex_take();
                    memcpy(&g_data_item_headers[curr_index], &tmp_hdr, DATA_ITEM_HEADER_SIZE);
                    nvdm_port_protect_mutex_give();
                }
                break;
            default:
                nvdm_log_msgid_error(nvdm_045, 3, data_item_header.status, src_pnum, pos);
                /* Mark the data behind it as garbage and no need to move the data behind. */
                return offset;
        }
    }

    return offset;
}

#ifndef __EXT_BOOTLOADER__
static nvdm_status_t reset_nvdm_parameter_check(const nvdm_item_id_t *array, uint32_t len, nvdm_reset_action_t action)
{
    nvdm_status_t check_res = NVDM_STATUS_OK;
    uint32_t idx = 0;
    const nvdm_item_id_t *cur = NULL;

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    if ((action > NVDM_ACTION_DELETE) || (len == 0) || (array == NULL)){
        return NVDM_STATUS_INVALID_PARAMETER;
    }
    for (idx = 0; idx < len; idx++){
        cur = array + idx;
        if(cur->group_name == NULL){
            check_res = NVDM_STATUS_INVALID_PARAMETER;
            break;
        }
        if((cur->group_name == NULL) && (cur->item_name == NULL)){
            check_res = NVDM_STATUS_INVALID_PARAMETER;
            break;
        }
        /* When group_name is not NULL and item_name is NULL, it means all items in group. */
    }

    return check_res;
}

static bool reset_nvdm_item_match(const nvdm_item_id_t *array, uint32_t len, const char *group_name, const char *item_name)
{
    bool item_is_match  = false;
    uint32_t idx = 0;
    for (idx = 0; idx < len; idx++) {
        if (array[idx].item_name == NULL){
            /* When group_name is not NULL and item_name is NULL, it means all items in group. */
            if (strcmp(array[idx].group_name, group_name) == 0){
                item_is_match = true;
                break;
            }
        } else {
            if ((strcmp(array[idx].group_name, group_name) == 0) && (strcmp(array[idx].item_name, item_name) == 0)) {
                item_is_match = true;
                break;
            }
        }
    }
    return item_is_match;
}

nvdm_status_t nvdm_reset_items(const nvdm_item_id_t *array, uint32_t len, nvdm_reset_action_t action)
{
    nvdm_status_t op_res = reset_nvdm_parameter_check(array, len, action);
    char *group_name = NULL, *item_name = NULL;
    bool item_is_match  = false;

    if (op_res != NVDM_STATUS_OK){
        return op_res;
    }

    /* align with nvdm_write_data_item API, there is a '\0' at the end of C string */
    group_name = (char *)nvdm_port_malloc(g_max_group_name_size + 1);
    item_name = (char *)nvdm_port_malloc(g_max_data_item_name_size + 1);
    if ((group_name == NULL) || (item_name == NULL)){
        if (group_name != NULL){
            nvdm_port_free(group_name);
        }
        if (item_name != NULL){
            nvdm_port_free(item_name);
        }
        return NVDM_STATUS_ERROR;
    }

    op_res = nvdm_query_begin();
    if (op_res != NVDM_STATUS_OK){
        nvdm_port_free(group_name);
        nvdm_port_free(item_name);
        return op_res;
    }
    while (nvdm_query_next_group_name(group_name) == NVDM_STATUS_OK) {
        while (nvdm_query_next_data_item_name(item_name) == NVDM_STATUS_OK) {
            item_is_match = reset_nvdm_item_match(array, len, group_name, item_name);
            if(action == NVDM_ACTION_KEEP){
                /* for NVDM_ACTION_KEEP, delete all items not in array */
                if(item_is_match == false){
                    nvdm_delete_data_item(group_name, item_name);
                }
            } else {
                /* for NVDM_ACTION_DELETE, delete all items in array */
                if(item_is_match == true){
                    nvdm_delete_data_item(group_name, item_name);
                }
            }
        }
    }
    op_res = nvdm_query_end();
    nvdm_port_free(group_name);
    nvdm_port_free(item_name);
    return op_res;
}
#endif /* #ifdef __EXT_BOOTLOADER__ */

#endif

