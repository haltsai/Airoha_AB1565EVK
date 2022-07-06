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
 
#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "memory_attribute.h"
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_opt.h"
#include "hal_flash_combo_init.h"
#include "hal_flash_combo_defs.h"
#include "hal_flash_mtd.h"
#include "hal_flash_mtd_internal.h"
#if defined(__SERIAL_FLASH__)
#include "hal_flash_sf.h"
#endif


//#include "br_GFH_cmem_id_info.h"
typedef struct {
    GFH_Header            m_gfh_hdr;
    CMEMEntryIDList       m_data;
} GFH_CMEM_ID_INFO_v1;


//-----------------------------------------------------------------------------
// MCP ID Table
//-----------------------------------------------------------------------------
#define COMBO_MEM_STRUCT_HEAD  COMBO_MEM_ID_GFH_HEADER, { COMBO_MEM_ID_VER, COMBO_MEM_DEVICE_COUNT, {
#define COMBO_MEM_STRUCT_FOOT  } }

const GFH_CMEM_ID_INFO_v1 combo_mem_id_list = {
    COMBO_MEM_STRUCT_HEAD
    #ifndef AB156X
    {
        // Fidelix 128
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xF8, 0x42, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //winbond 256 without QPI
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x60, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //winbond 256 with QPI
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x80, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //MX25U12832F
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC2, 0x25, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //MX25U25645G
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC2, 0x25, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    #endif
    {
        // winbond 128 without QPI
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x60, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //winbond 128 with QPI
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x80, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //GD25LE128E
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC8, 0x60, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    #ifdef AB156X
    {
        // winbond 64 without QPI
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x60, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        // winbond 64 with QPI
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x80, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //winbond 32 without QPI
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x60, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //GD25LE64E
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC8, 0x60, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {
        //GD25LE32D
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC8, 0x60, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    }
    #endif
    COMBO_MEM_STRUCT_FOOT
};


int32_t CMEM_EMIINIT_Index(void);

void CMEM_EMIINIT_ReadID(const uint16_t CS, void *BaseAddr, uint16_t *flashid);


static signed short int cmem_emiinit_index = -1;
static signed short int cmem_index = -1;

uint8_t nor_id[4];

bool CMEM_CheckValidDeviceID(uint8_t *id)
{
    int32_t i, j;
    const CMEMEntryID *id_list = NULL;

    id_list = combo_mem_id_list.m_data.List;
	
    for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++)	{
        /*Compare ID*/
        for (j = 0; j < id_list[i].IDLength; j++) {
            if (id_list[i].ID[j] != id[j]) {
                break;
            }
        }

        if (j == id_list[i].IDLength)   {
            return true;
        }
    }

    return false;
}

ATTR_TEXT_IN_RAM void NOR_ReadID(const uint16_t CS, volatile uint16_t *BaseAddr, uint16_t *flashid)
{
    uint32_t savedMask;

    // Read Serial Flash ID
    {
        uint8_t cmd, id[SF_FLASH_ID_LENGTH], i;

        cmd = SF_CMD_READ_ID_QPI;
        savedMask = SaveAndSetIRQMask();
        SFI_Dev_Command_Ext(CS, &cmd, id, 1, SF_FLASH_ID_LENGTH);
        RestoreIRQMask(savedMask);

        if (id[0] == 0x00 || id[0] == 0xFF || (CMEM_CheckValidDeviceID(id) == false)) {
            cmd = SF_CMD_READ_ID;
            savedMask = SaveAndSetIRQMask();
            SFI_Dev_Command_Ext(CS, &cmd, id, 1, SF_FLASH_ID_LENGTH);
            RestoreIRQMask(savedMask);
        }

        for (i = 0; i < SF_FLASH_ID_LENGTH; i++) {
            flashid[i] = id[i];
        }
    }
    return;
}



int32_t CMEM_Index()
{
    int32_t i, j;
    CMEMFlashID id;
    const CMEMEntryID *id_list = NULL;

    if (cmem_index >= 0) {
        return cmem_index;
    }

    NOR_ReadID(0, (void *)(SFC_GENERIC_FLASH_BANK_MASK), id.NOR);

    nor_id[0] = id.NOR[0];
    nor_id[1] = id.NOR[1];
    nor_id[2] = id.NOR[2];
    nor_id[3] = id.NOR[3];
    id_list = combo_mem_id_list.m_data.List;

    // seach CMEM list for specific Flash ID
    for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++)    {
        // Check 1: Compare ID
        for (j = 0; j < id_list[i].IDLength; j++) {
            if (id_list[i].ID[j] != id.NOR[j]) {
                break;
            }
        }
        // Check 2: Compare RegionInfo
        if (j == id_list[i].IDLength)   {
            // TBD: Compare RegionInfo
            cmem_index = i;
            break;  // entry found, break the loop
        }
    }

    return cmem_index;
}

const CMEMEntryID *CMEM_GetIDEntry(uint32_t index)
{
    return &combo_mem_id_list.m_data.List[index];
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Read Flash ID
  @param[in] BaseAddr Base address to the Flash
  @param[out] flashid Flash ID
  @remarks
    This function is only allowed in EMI/SFI init stage.
*/
ATTR_TEXT_IN_SYSRAM void CMEM_EMIINIT_ReadID(const uint16_t CS, void *BaseAddr, uint16_t *flashid)
{
    // Serial Flash
    {
        uint8_t cmd, id[SF_FLASH_ID_LENGTH], i;

        cmd = SF_CMD_READ_ID_QPI;
        SFI_Dev_Command_Ext(CS, &cmd, id, 1, SF_FLASH_ID_LENGTH);

        if (id[0] == 0x00 || id[0] == 0xFF || (CMEM_EMIINIT_CheckValidDeviceID(id) == false)) {
            cmd = SF_CMD_READ_ID;
            SFI_Dev_Command_Ext(CS, &cmd, id, 1, SF_FLASH_ID_LENGTH);
        }

        for (i = 0; i < SF_FLASH_ID_LENGTH; i++) {
            flashid[i] = id[i];
        }
    }
    return;
}


//-----------------------------------------------------------------------------
/*!
  @brief
    Determine whether SF ID is valid.
    Apply for MT6250 MT6260/61 because after command issue(ex: Read ID), Data pins are in floating, may read trasient value instead of 0x00 or 0xFF.
  @retval
    true: the device ID0 is valid.
    false: the device ID0 is not valid.
*/
ATTR_TEXT_IN_SYSRAM bool CMEM_EMIINIT_CheckValidDeviceID(uint8_t *id)
{
    int32_t i, j;
    const CMEMEntryID *id_list = NULL;
    // seach CMEM list for specific Flash ID
    id_list = combo_mem_id_list.m_data.List;

    for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++)    {
        // Check 1: Compare ID
        for (j = 0; j < id_list[i].IDLength; j++) {
            if (id_list[i].ID[j] != id[j]) {
                break;
            }
        }
        // Check 2: Compare RegionInfo
        if (j == id_list[i].IDLength)   {
            // TBD: Compare RegionInfo
            return true;
        }
    }

    return false;
}


ATTR_TEXT_IN_SYSRAM int32_t CMEM_EMIINIT_Index()
{
    int32_t i, j;
    CMEMFlashID id;
    const CMEMEntryID *id_list = NULL;

    if (cmem_emiinit_index >= 0) {
        return cmem_emiinit_index;
    }
    // Read Flash ID
    CMEM_EMIINIT_ReadID(0, (void *)NOR_FLASH_BASE_ADDRESS, id.NOR);

// seach CMEM list for specific Flash ID
    id_list = combo_mem_id_list.m_data.List;
    nor_id[0] = id.NOR[0];
    nor_id[1] = id.NOR[1];
    nor_id[2] = id.NOR[2];
    nor_id[3] = id.NOR[3];
    for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++) {
        // Check 1: Compare ID
        for (j = 0; j < id_list[i].IDLength; j++) {
            if (id_list[i].ID[j] != id.NOR[j]) {
                break;
            }
        }
        // Check 2: Compare RegionInfo
        if (j == id_list[i].IDLength)   {
            // TBD: Compare RegionInfo
            cmem_emiinit_index = i;
            break;
        }
    }
    return cmem_emiinit_index;  // entry not found
}


#endif // HAL_FLASH_MODULE_ENABLED


