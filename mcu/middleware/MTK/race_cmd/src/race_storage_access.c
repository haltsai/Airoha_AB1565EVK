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


#include "race_cmd_feature.h"
#ifdef RACE_STORAGE_CMD_ENABLE
#ifdef RACE_FOTA_CMD_ENABLE
#include "fota_flash.h"
#endif
#include "race_xport.h"
#include "race_storage_access.h"


typedef struct
{
    race_storage_module_enum module;
    uint8_t storage_type;
    uint32_t address;
    uint32_t length;
}race_storage_access_info_struct;


/* Handle internal flash only */
race_storage_access_info_struct g_race_storage_access_info[] = 
{
    /* Add accessiable internal flash address range above this line. */
    {RACE_STORAGE_MODULE_FOTA, 0, 0, 0},
    {RACE_STORAGE_MODULE_FOTA, 1, 0, 0},

    {RACE_STORAGE_MODULE_NONE, 0, 0, 0}
};


RACE_ERRCODE race_storage_update_accessible_addr(race_storage_module_enum module,
                                                            uint8_t storage_type,
                                                            uint32_t address,
                                                            uint32_t length)

{
    uint32_t i = 0;

    if (RACE_STORAGE_MODULE_NONE >= module || 
        RACE_STORAGE_MODULE_MAX <= module || 
        (0 != storage_type
#ifdef RACE_STORAGE_BSP_FLASH_ENABLE
        && 1 != storage_type
#endif
        ))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    while (RACE_STORAGE_MODULE_NONE != g_race_storage_access_info[i].module)
    {
        if (storage_type == g_race_storage_access_info[i].storage_type &&
            module == g_race_storage_access_info[i].module)
        {
            g_race_storage_access_info[i].address = address;
            g_race_storage_access_info[i].length = length;
            return RACE_ERRCODE_SUCCESS;
        }
        
        i++;
    }

    return RACE_ERRCODE_FAIL;
}


RACE_ERRCODE race_storage_is_addr_accessible(uint8_t storage_type,
                                                      uint32_t address,
                                                      uint32_t length)
{
    uint32_t i = 0;

    if (!length)
    {
        return RACE_ERRCODE_SUCCESS;
    }

    if ( 0 != storage_type
#ifdef RACE_STORAGE_BSP_FLASH_ENABLE
        && 1 != storage_type
#endif
        )
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    while (RACE_STORAGE_MODULE_NONE != g_race_storage_access_info[i].module)
    {
        if (storage_type == g_race_storage_access_info[i].storage_type &&
            address >= g_race_storage_access_info[i].address &&
            address + length <= g_race_storage_access_info[i].address + g_race_storage_access_info[i].length)
        {
            return RACE_ERRCODE_SUCCESS;
        }

        i++;
    }

    RACE_LOG_MSGID_I("Not accessible. storage_type:%d address:%x length:%x",3,
               storage_type, address, length);

    return RACE_ERRCODE_NOT_SUPPORT;
}


#ifdef RACE_FOTA_CMD_ENABLE
RACE_ERRCODE race_storage_enable_fota_partition_accessibility(void)
{
    FotaStorageType storage_type;
    uint32_t fota_address, length;
    FOTA_ERRCODE ret = FOTA_ERRCODE_FAIL;

    ret = fota_flash_get_fota_partition_info(&storage_type, &fota_address, &length);
    if (FOTA_ERRCODE_SUCCESS == ret)
    {
        if (InternalFlash == storage_type)
        {
            return race_storage_update_accessible_addr(RACE_STORAGE_MODULE_FOTA,
                                                       0,
                                                       fota_address,
                                                       length);
        }
        else if (ExternalFlash == storage_type)
        {
            return race_storage_update_accessible_addr(RACE_STORAGE_MODULE_FOTA,
                                                       1,
                                                       fota_address,
                                                       length);
        }
    }

    return RACE_ERRCODE_FAIL;
}


RACE_ERRCODE race_storage_disable_fota_partition_accessibility(void)
{
    FotaStorageType storage_type;
    uint32_t fota_address, length;
    FOTA_ERRCODE ret = FOTA_ERRCODE_FAIL;

    ret = fota_flash_get_fota_partition_info(&storage_type, &fota_address, &length);
    if (FOTA_ERRCODE_SUCCESS == ret)
    {
        if (InternalFlash == storage_type)
        {
            return race_storage_update_accessible_addr(RACE_STORAGE_MODULE_FOTA,
                                                       0,
                                                       0,
                                                       0);
        }
        else if (ExternalFlash == storage_type)
        {
            return race_storage_update_accessible_addr(RACE_STORAGE_MODULE_FOTA,
                                                       1,
                                                       0,
                                                       0);
        }
    }

    return RACE_ERRCODE_FAIL;
}
#endif /* RACE_FOTA_CMD_ENABLE */
#endif /* RACE_STORAGE_CMD_ENABLE */

