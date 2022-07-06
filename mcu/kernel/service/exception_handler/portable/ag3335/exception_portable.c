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

/* Includes ------------------------------------------------------------------*/
#include "exception_config.h"
#include "exception_handler.h"
#include "exception_portable.h"
#include "syslog.h"
#include "mux.h"
#include "hal_core_status.h"
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
#ifdef MTK_FOTA_VIA_RACE_CMD
#include "fota_multi_info.h"
#endif /* MTK_FOTA_VIA_RACE_CMD */
#endif /* EXCEPTION_MEMDUMP_MODE */


/* Public define -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
void exception_log_service_init(void)
{
    /* feed wdt to keep time for log service init */
    exception_feed_wdt();

    exception_syslog_callback();
    //exception_mux_callback();
}

void exception_log_service_post_process(void)
{
    /* feed wdt to keep time for log service post process*/
    exception_feed_wdt();

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
#if defined(MTK_SAVE_LOG_TO_FLASH_ENABLE) || defined(MTK_ONLINE_LOG_TO_APK_ENABLE)
    extern void exception_syslog_offline_dump_callback(void);
    exception_syslog_offline_dump_callback();
#endif /* MTK_SAVE_LOG_TO_FLASH_ENABLE */
#endif /* (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP) */
}

void exception_core_status_update(void)
{
    hal_core_status_write(HAL_CORE_CM4, HAL_CORE_EXCEPTION);
}

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
void exception_minidump_header_update(void)
{
    minidump_header.name[0]  = 'C';
    minidump_header.name[1]  = 'M';
    minidump_header.name[2]  = '4';
    minidump_header.name[3]  = ' ';
    minidump_header.name[4]  = 'M';
    minidump_header.name[5]  = 'I';
    minidump_header.name[6]  = 'N';
    minidump_header.name[7]  = 'I';
    minidump_header.name[8]  = 'D';
    minidump_header.name[9]  = 'U';
    minidump_header.name[10] = 'M';
    minidump_header.name[11] = 'P';
    minidump_header.name[12] = 'S';
    minidump_header.name[13] = 'U';
    minidump_header.name[14] = 'C';
    minidump_header.name[15] = 'C';
    minidump_header.name[16] = 'E';
    minidump_header.name[17] = 'E';
    minidump_header.name[18] = 'D';
    minidump_header.name[19] = 0;
    minidump_header.length = MINIDUMP_DATA_SIZE_CM4;
    minidump_header.core_num = 1 + EXCEPTION_SLAVES_TOTAL;
    minidump_header.core_size[0] = MINIDUMP_DATA_SIZE_CM4;
}

void exception_minidump_fota_status_clean(void)
{
#if defined(MTK_FOTA_VIA_RACE_CMD) && !defined(MTK_FOTA_STORE_IN_EXTERNAL_FLASH)
    /* feed wdt to keep time for cleaning fota status */
    exception_feed_wdt();
    /* clean FOTA status to prevent FOTA upgrade after reboot */
    fota_state_write(0x0);
#endif /* defined(MTK_FOTA_VIA_RACE_CMD) && !defined(MTK_FOTA_STORE_IN_EXTERNAL_FLASH) */
}
#endif /* EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP */

void exception_msgid_portable(uint8_t cpu_id, const char *message, uint32_t arg_cnt, ...)
{
    /* Just to avoid compiler warnings. */
    ( void ) cpu_id;
    ( void ) message;
    ( void ) arg_cnt;
}
