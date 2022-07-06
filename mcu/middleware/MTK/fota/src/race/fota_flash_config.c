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

/**
 * File: fota_flash_config.c
 *
 * Description: This file obtains the memory layout from the partition table.
 *
 * Note: See doc/Airoha_IoT_SDK_Firmware_Update_Developers_Guide.pdf for more detail.
 *
 */


#include "fota_flash.h"
#if FOTA_STORE_IN_EXTERNAL_FLASH
#include "bsp_flash.h"
#endif
#include "fota_platform.h"


#if (PRODUCT_VERSION == 2523)
/* The variable stores the flash partition information. It only stores the information of the FOTA partition and the partitons which can be updated by FOTA. */
static fota_flash_partition_info s_flash_table[] = {
#ifdef BL_FOTA_ENABLE
    {
        .partition_type = FLASH_PARTITION_TYPE_CM4_FW,
        .LoadAddressHigh = 0,
        .LoadAddressLow = CM4_BASE - FLASH_CONFIG_BASE,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = CM4_LENGTH,
        .is_external = FALSE
    },
#endif
    {
        .partition_type = FLASH_PARTITION_TYPE_FOTA,
        .LoadAddressHigh = 0,
        .LoadAddressLow = FOTA_RESERVED_BASE - FLASH_CONFIG_BASE,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = FOTA_RESERVED_LENGTH,  /* The last block of fota temp partition is reserved for triggered flag & update info */
        .is_external = FALSE
    },
    {
        .partition_type = FLASH_PARTITION_TYPE_FOTA_EXT,
        .LoadAddressHigh = 0,
        .LoadAddressLow = FOTA_RESERVED_BASE - FLASH_CONFIG_BASE,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = FOTA_RESERVED_LENGTH,  /* The last block of fota temp partition is reserved for triggered flag & update info */
        .is_external = TRUE
    },
    {
        .partition_type = FLASH_PARTITION_TYPE_MAX,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    }
};

#else
/* The variable stores the flash partition information. It only stores the information of the FOTA partition and the partitons which can be updated by FOTA. */
static fota_flash_partition_info s_flash_table[] = {
#ifdef BL_FOTA_ENABLE
#ifdef FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
{
        .partition_type = FLASH_PARTITION_TYPE_PARTITION_TABLE,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#if defined(MTK_SECURE_BOOT_ENABLE)
    {
        .partition_type = FLASH_PARTITION_TYPE_SECURITY_HEADER1,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
    {
        .partition_type = FLASH_PARTITION_TYPE_BOOTLOADER,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#if (PRODUCT_VERSION == 3335 || PRODUCT_VERSION == 2822 || defined(AB156X))
    {
        .partition_type = FLASH_PARTITION_TYPE_SECURITY_HEADER2,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#if (PRODUCT_VERSION != 3335 && PRODUCT_VERSION != 2822 && !defined(AB156X))
    {
        .partition_type = FLASH_PARTITION_TYPE_N9,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
    {
        .partition_type = FLASH_PARTITION_TYPE_CM4_FW,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#if (PRODUCT_VERSION != 3335)
    {
        .partition_type = FLASH_PARTITION_TYPE_DSP0,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#if (PRODUCT_VERSION != 3335 && PRODUCT_VERSION != 2822 && !defined(AB156X))
    {
        .partition_type = FLASH_PARTITION_TYPE_DSP1,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
    {
        .partition_type = FLASH_PARTITION_TYPE_NVDM,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#if ((PRODUCT_VERSION == 1552) || (defined(AM255X)) || (PRODUCT_VERSION == 2822) || defined(AB156X)) && defined(ROFS_BASE)
    {
        .partition_type = FLASH_PARTITION_TYPE_ROFS,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#if (PRODUCT_VERSION == 3335)
    {
        .partition_type = FLASH_PARTITION_TYPE_FS,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
    {
        .partition_type = FLASH_PARTITION_TYPE_GNSS_CONFIG,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#if ((PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 2821))
#ifdef LM_GVA_BASE
    {
        .partition_type = FLASH_PARTITION_TYPE_LM_GVA,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif

#ifdef LM_AMA_BASE
    {
        .partition_type = FLASH_PARTITION_TYPE_LM_AMA,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#elif (defined(AB156X) && defined(LM_BASE))
    {
        .partition_type = FLASH_PARTITION_TYPE_LM,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#ifdef ANC_FW_BASE
    {
        .partition_type = FLASH_PARTITION_TYPE_ANC_FW,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#ifdef ANC_FW_TUNE_BASE
    {
        .partition_type = FLASH_PARTITION_TYPE_ANC_FW_TUNE,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#ifdef GRAMMAR_MODEL1_BASE
    {
        .partition_type = FLASH_PARTITION_TYPE_GRAMMAR_MODEL1,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#ifdef GRAMMAR_MODEL2_BASE
    {
        .partition_type = FLASH_PARTITION_TYPE_GRAMMAR_MODEL2,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#ifdef MIC_DATA_RECORD_BASE
    {
        .partition_type = FLASH_PARTITION_TYPE_MIC_DATA_RECORD,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    },
#endif
#endif /* BL_FOTA_ENABLE */
    {
        .partition_type = FLASH_PARTITION_TYPE_FOTA,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,  /* The last block of fota temp partition is reserved for triggered flag & update info */
        .is_external = FALSE
    },
    {
        /* FOTA_EXT is not included in the partition table. */
        .partition_type = FLASH_PARTITION_TYPE_FOTA_EXT,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,  /* The last block of fota temp partition is reserved for triggered flag & update info */
        .is_external = TRUE
    },
    {
        .partition_type = FLASH_PARTITION_TYPE_MAX,
        .LoadAddressHigh = 0,
        .LoadAddressLow = 0,
        .BinaryLengthHigh = 0,
        .BinaryLengthLow = 0,
        .is_external = FALSE
    }
};
#endif


bool fota_flash_config_init(fota_flash_partition_info **partition_info)
{
#if (PRODUCT_VERSION != 2523)
    int i = 0;
#endif

    if (!partition_info || *partition_info) {
        return false;
    }

#if (PRODUCT_VERSION != 2523)

    while (FLASH_PARTITION_TYPE_MAX != s_flash_table[i].partition_type) {
        switch (s_flash_table[i].partition_type) {
#ifdef BL_FOTA_ENABLE
#ifdef FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
        case FLASH_PARTITION_TYPE_PARTITION_TABLE: {
            s_flash_table[i].LoadAddressLow = (uint32_t)PARTITION_TABLE - FLASH_CONFIG_BASE;
            s_flash_table[i].BinaryLengthLow = 0x00001000;
            break;
        }
#if defined(MTK_SECURE_BOOT_ENABLE)
        case FLASH_PARTITION_TYPE_SECURITY_HEADER1: {
            s_flash_table[i].LoadAddressLow = SEC_HEADER1_BASE - FLASH_CONFIG_BASE;
            s_flash_table[i].BinaryLengthLow = SEC_HEADER1_LENGTH;
            break;
        }
#endif
        case FLASH_PARTITION_TYPE_BOOTLOADER: {
            s_flash_table[i].LoadAddressLow = BL_BASE - FLASH_CONFIG_BASE;
            s_flash_table[i].BinaryLengthLow = BL_LENGTH;
            break;
        }
#endif
#if (PRODUCT_VERSION == 3335 || PRODUCT_VERSION == 2822 || defined(AB156X))
            case FLASH_PARTITION_TYPE_SECURITY_HEADER2: {
                s_flash_table[i].LoadAddressLow = SEC_HEADER2_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = SEC_HEADER2_LENGTH;
                break;
            }
#endif
#if (PRODUCT_VERSION != 3335 && PRODUCT_VERSION != 2822 && !defined(AB156X))
            case FLASH_PARTITION_TYPE_N9: {
                s_flash_table[i].LoadAddressLow = N9_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = N9_LENGTH;
                break;
            }
#endif

            case FLASH_PARTITION_TYPE_CM4_FW: {
                s_flash_table[i].LoadAddressLow = CM4_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = CM4_LENGTH;
                break;
            }

#if (PRODUCT_VERSION != 3335)
            case FLASH_PARTITION_TYPE_DSP0: {
                s_flash_table[i].LoadAddressLow = DSP0_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = DSP0_LENGTH;
                break;
            }
#endif

#if (PRODUCT_VERSION != 3335 && PRODUCT_VERSION != 2822 && !defined(AB156X))
            case FLASH_PARTITION_TYPE_DSP1: {
                s_flash_table[i].LoadAddressLow = DSP1_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = DSP1_LENGTH;
                break;
            }
#endif

            case FLASH_PARTITION_TYPE_NVDM: {
                s_flash_table[i].LoadAddressLow = ROM_NVDM_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = ROM_NVDM_LENGTH;
                break;
            }

#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822) || defined(AB156X)) && defined(ROFS_BASE)
            case FLASH_PARTITION_TYPE_ROFS: {
                s_flash_table[i].LoadAddressLow = ROFS_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = ROFS_LENGTH;
                break;
            }
#endif

#if (PRODUCT_VERSION == 3335)
            case FLASH_PARTITION_TYPE_FS: {
                s_flash_table[i].LoadAddressLow = FS_RESERVED_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = FS_RESERVED_LENGTH;
                break;
            }

            case FLASH_PARTITION_TYPE_GNSS_CONFIG: {
                s_flash_table[i].LoadAddressLow = GNSS_CONFIG_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = GNSS_CONFIG_LENGTH;
                break;
            }
#endif

#if ((PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 2821))
#ifdef LM_GVA_BASE
            case FLASH_PARTITION_TYPE_LM_GVA: {
                s_flash_table[i].LoadAddressLow = LM_GVA_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = LM_GVA_LENGTH;
                break;
            }
#endif

#ifdef LM_AMA_BASE
            case FLASH_PARTITION_TYPE_LM_AMA: {
                s_flash_table[i].LoadAddressLow = LM_AMA_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = LM_AMA_LENGTH;
                break;
            }
#endif

#elif (defined(AB156X) && defined(LM_BASE))
            case FLASH_PARTITION_TYPE_LM: {
                s_flash_table[i].LoadAddressLow = LM_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = LM_LENGTH;
                break;
            }
#endif

#if defined(ANC_FW_BASE)
            case FLASH_PARTITION_TYPE_ANC_FW: {
                s_flash_table[i].LoadAddressLow = ANC_FW_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = ANC_FW_LENGTH;
                break;
            }
#endif

#if defined(ANC_FW_TUNE_BASE)
            case FLASH_PARTITION_TYPE_ANC_FW_TUNE: {
                s_flash_table[i].LoadAddressLow = ANC_FW_TUNE_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = ANC_FW_TUNE_LENGTH;
                break;
            }
#endif

#if defined(GRAMMAR_MODEL1_BASE)
            case FLASH_PARTITION_TYPE_GRAMMAR_MODEL1: {
                s_flash_table[i].LoadAddressLow = GRAMMAR_MODEL1_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = GRAMMAR_MODEL1_LENGTH;
                break;
            }
#endif

#if defined(GRAMMAR_MODEL2_BASE)
            case FLASH_PARTITION_TYPE_GRAMMAR_MODEL2: {
                s_flash_table[i].LoadAddressLow = GRAMMAR_MODEL2_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = GRAMMAR_MODEL2_LENGTH;
                break;
            }
#endif

#if defined(MIC_DATA_RECORD_BASE)
            case FLASH_PARTITION_TYPE_MIC_DATA_RECORD: {
                s_flash_table[i].LoadAddressLow = MIC_DATA_RECORD_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = MIC_DATA_RECORD_LENGTH;
                break;
            }
#endif
#endif /* BL_FOTA_ENABLE */

            case FLASH_PARTITION_TYPE_FOTA: {
                s_flash_table[i].LoadAddressLow = FOTA_RESERVED_BASE - FLASH_CONFIG_BASE;
                s_flash_table[i].BinaryLengthLow = FOTA_RESERVED_LENGTH;
                break;
            }

            case FLASH_PARTITION_TYPE_FOTA_EXT: {
#if FOTA_STORE_IN_EXTERNAL_FLASH
                s_flash_table[i].LoadAddressLow = FOTA_EXT_RESERVED_BASE_DEFAULT & (~SPI_SERIAL_FLASH_ADDRESS);
#else
                s_flash_table[i].LoadAddressLow = FOTA_EXT_RESERVED_BASE_DEFAULT;
#endif
                s_flash_table[i].BinaryLengthLow = FOTA_EXT_RESERVED_LENGTH_DEFAULT;
                break;
            }

            default: {
                FOTA_LOG_MSGID_W("Unknown partition type:%d", 1, s_flash_table[i].partition_type);
                return false;
            }
        }
        i++;
    }
#endif

    *partition_info = s_flash_table;
    return true;
}


FOTA_ERRCODE fota_flash_config_fota_partition_in_external_flash(uint32_t start_address,
                                                                uint32_t length)
{
#if FOTA_STORE_IN_EXTERNAL_FLASH
    uint32_t i = 0;

    if (!length) {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    FOTA_LOG_MSGID_I("FOTA ext. flash start_address:%x", 1, start_address);

    while (FLASH_PARTITION_TYPE_MAX != s_flash_table[i].partition_type) {
        if (FLASH_PARTITION_TYPE_FOTA_EXT == s_flash_table[i].partition_type) {
            s_flash_table[i].LoadAddressLow = start_address & (~SPI_SERIAL_FLASH_ADDRESS);
            s_flash_table[i].BinaryLengthLow = length;
            return FOTA_ERRCODE_SUCCESS;
        }
        i++;
    }

    return FOTA_ERRCODE_FAIL;
#else
    return FOTA_ERRCODE_UNSUPPORTED;
#endif
}

