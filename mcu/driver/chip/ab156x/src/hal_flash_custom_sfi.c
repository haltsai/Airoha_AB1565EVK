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
#include "ab156x.h"
#include "hal_flash_sf.h"
#include "hal_flash_opt.h"
#include "hal_flash_combo_init.h"
#include "hal_flash_combo_sfi_defs.h"
#include "hal_flash_custom_sfi.h"


/*************************************************************************
* FUNCTION
*  custom_setSFI()
*
* DESCRIPTION
*   This routine aims to set SFI
*
* PARAMETERS
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/

//-----------------------------------------------------------------------------
// MCP Serial Flash EMI/SFI settings
//-----------------------------------------------------------------------------

// include type defines (to be modified by EMI/SFI owner)
ATTR_RODATA_IN_RAM

const CMEMEntrySFIList combo_mem_hw_list = {   // (to be renamed by SFI owner)
    "COMBO_MEM_SFI",
    COMBO_SFI_VER,           // SFI structure version
    SFI_COMBO_COUNT,   // defined in custom_Memorydevice.h
    {
        #ifndef AB156X
        {
            // MTKSIP_2523_SF_32_04
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x06000006,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0x0C0B7771,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_04
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5772,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x06000006,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SPI,1, 0xB7, SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0x0C0B7771,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_01
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5772,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x06000006,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SPI,1, 0xB7, SF_UNDEF, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MX25U12832F
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x05000005,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x06, SPI, 2, 0x01, 0x42, SPI,
                2, 0x05, 0x01, SPI, 2, 0xC0, 0x02, SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MX25U25645G
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5772,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x05000005,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x06, SPI,  2, 0x01, 0x42, SPI,
                2, 0x05, 0x01, SPI, 2, 0xC0, 0x02, SPI,
                1, 0xB7, SF_UNDEF,  0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        #endif
        {
            // MTKSIP_2523_SF_32_04
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x06000006,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0x0C0B7771,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_01
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x06000006,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // GD25LE128E
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x05000005,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        #ifdef AB156X
        {
            // MTKSIP_2523_SF_32_04
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x06000006,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0x0C0B7771,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_01
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x06000006,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_01
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x06000006,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // GD25LE64E
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x05000005,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // GD25LE32D
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000000,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x05000005,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SF_UNDEF, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        }
        #endif
    }
};



/* SFC Driving registers */
#define SFIO_CFG0_DRV       ((volatile unsigned int *)(TOP_MISC_CFG_BASE+0x200))
#define SFIO_CFG1_DRV       ((volatile unsigned int *)(TOP_MISC_CFG_BASE+0x204))
#define SFIO_CFG2_DRV       ((volatile unsigned int *)(TOP_MISC_CFG_BASE+0x208))
#define SFIO_CFG3_DRV       ((volatile unsigned int *)(TOP_MISC_CFG_BASE+0x20C))
#define SFIO_CFG4_DRV       ((volatile unsigned int *)(TOP_MISC_CFG_BASE+0x210))
#define SFIO_CFG5_DRV       ((volatile unsigned int *)(TOP_MISC_CFG_BASE+0x214))


#define SFC_RW_SF_DLY_CTL3_ADD ((volatile unsigned int *)(SFC_BASE +0x28))
#define SFC_RW_SF_MISC_CTL_ADD ((volatile unsigned int *)(SFC_BASE +0x8))

/* SFC GPIO registers */
//#define SFIO_CFG0           (TOP_MISC_CFG_BASE + 0xA4)
//#define SFIO_CFG1           (TOP_MISC_CFG_BASE + 0xA8)
//#define SFIO_CFG2           (TOP_MISC_CFG_BASE + 0xAC)

#define SF_NORMAL_HIGHZ     (0xFFCFFFCF)

/* SFC SLT registers */
#define SF_SLT_MODE         (0x00100000)
#define SYSTEM_INFOD        ((volatile unsigned int *)(TOP_MISC_CFG_BASE+0xA0))

#define GPIO_RESEN0_1       (GPIO_BASE + 0xb10)
#define GPIO_RESEN1_1       (GPIO_BASE + 0xb30)
#define SF_SLT_HIGHZ        (0xFFFFF03F)

#define GPIO_DRV1           (GPIO_BASE + 0x820)
#define SF_SLT_DRIVING_CLR  (0xFF000FFF)
#define SF_SLT_DRIVING_12mA (0xFFAAAFFF)

int sfi_index, CS_COUNT_SFI;
ATTR_TEXT_IN_RAM signed char custom_setSFI(void)
{
    const CMEMEntrySFI *sfi;

    //*(SFC_RW_SF_DLY_CTL3_ADD) = 0x18;
    // it can't read ID @104 Mhz on 1562 and later chip. Adjust it if boot up failed.
    *(SFC_RW_SF_DLY_CTL3_ADD) = 0x06000006;
    *(SFC_RW_SF_MISC_CTL_ADD) = (*(SFC_RW_SF_MISC_CTL_ADD) | 0x10);

    sfi_index = CMEM_EMIINIT_Index(); // API CMCP_EMIINIT_Index() is defined in hal_flash_combo_init.h

    if (sfi_index < 0){
        // Add error handler here
#ifdef __EXT_BOOTLOADER__
        return 0;      //output error information if it is in bootloader
#endif
        while (1);
    }

    /*--------------------------------------------------------------------------
     * Step 1.
     * 1. Switch mode QPI/SPI Quad
     * 2. Set Burst/Wrap length
     *--------------------------------------------------------------------------*/
    sfi = &combo_mem_hw_list.List[sfi_index]; // the structure name "combo_mem_hw_list" can be renamed by SFI owner

#if defined(__SFI_CLK_78MHZ__) || defined(__SFI_CLK_80MHZ__)
    /* Device Initialization */
    SFI_Dev_Command_List(0, sfi->DevInit_78M);

    /*--------------------------------------------------------------------------
     * Step 2. Initialize Serial Flash Control Registers
     *--------------------------------------------------------------------------*/
    SFC->RW_SF_MAC_CTL = sfi->HWConf_78M[SFC_MAC_CTL];
    SFC->RW_SF_DIRECT_CTL = sfi->HWConf_78M[SFC_DR_CTL];

    SFC->RW_SF_MISC_CTL = sfi->HWConf_78M[SFC_MISC_CTL];
    SFC->RW_SF_MISC_CTL2 = sfi->HWConf_78M[SFC_MISC_CTL2];
    SFC->RW_SF_DLY_CTL2 = sfi->HWConf_78M[SFC_DLY_CTL2];
    SFC->RW_SF_DLY_CTL3 = sfi->HWConf_78M[SFC_DLY_CTL3];

    /* from datasheet default setting is '0' which means 4mA, use default setting. */
    #if 0
    uint32_t tmp_driving;
    tmp_driving = *SFIO_CFG0_DRV & 0xFFFFFFF8;
    *SFIO_CFG0_DRV = tmp_driving | sfi->HWConf_78M[SFC_Driving];
    tmp_driving = *SFIO_CFG1_DRV & 0xFFFFFFF8;
    *SFIO_CFG1_DRV = tmp_driving | sfi->HWConf_78M[SFC_Driving];
    tmp_driving = *SFIO_CFG2_DRV & 0xFFFFFFF8;
    *SFIO_CFG2_DRV = tmp_driving | sfi->HWConf_78M[SFC_Driving];
    tmp_driving = *SFIO_CFG3_DRV & 0xFFFFFFF8;
    *SFIO_CFG3_DRV = tmp_driving | sfi->HWConf_78M[SFC_Driving];
    tmp_driving = *SFIO_CFG4_DRV & 0xFFFFFFF8;
    *SFIO_CFG4_DRV = tmp_driving | sfi->HWConf_78M[SFC_Driving];
    tmp_driving = *SFIO_CFG5_DRV & 0xFFFFFFF8;
    *SFIO_CFG5_DRV = tmp_driving | sfi->HWConf_78M[SFC_Driving];
    #endif

#else
#error "Undefined BB chips of SFC 130MHz"
#endif /* __SFI_CLK_78MHZ__ */

    /*--------------------------------------------------------------------------
     * Step 3. EFuse Post process of IO driving/ Sample clk delay/ Input delay
     * Currently, only for MT6250 and MT6260
     *--------------------------------------------------------------------------*/

    return 0;

}


#define CLK_CONDB               ((volatile unsigned int *)(CONFIG_BASE+0x0104))
#define ACFG_CLK_UPDATE         ((volatile unsigned short *)(ANA_CFGSYS_BASE+0x0150))

#define RG_VCORE_VOSEL   ((volatile unsigned short *)(PMU_SD_base + 0x230))
#define RG_VSF_VOSEL     ((volatile unsigned short *)(PMU_SD_base + 0x1D0))



ATTR_TEXT_IN_RAM int custom_setSFIExt()
{

    // init SFI & SF device (QPI / wrap ...etc)
    custom_setSFI();

    return 0;
}


#else   /* ! HAL_FLASH_MODULE_ENABLED*/

#include "memory_attribute.h"
ATTR_TEXT_IN_SYSRAM signed char custom_setSFI(void)
{
    return 0;
}

ATTR_TEXT_IN_SYSRAM int custom_setSFIExt()
{
    return 0;
}

#endif //#ifdef HAL_FLASH_MODULE_ENABLED
