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

#ifdef MTK_BOOTREASON_CHECK_ENABLE
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bootreason_check.h"
#include "bootreason_portable.h"
#include "hal_rtc.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "memory_attribute.h"
#include "exception_handler.h"
#include "hal_log.h"

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const char *bootreason_string[] = {  "UNKNOWN",
                                            "NORMAL",
                                            "ASSERT",
                                            "EXCEPTION",
                                            "WATCHDOG",
                                            "SLEEPERROR",
                                            "WATCHDOG_RESET",
                                            "SOFT_RESET",
                                            "XOFF_RESET"
                                         };

#if defined(AB1565)
static const char *pmu_power_on_reason[] = {"UNKNOWN", "REGEN KEY POWER ON", "RTC ALARM", "UNKNOWN", "CHARGER POWER ON",
                                            "UNKNOWN", "UNKNOWN", "UNKNOWN", "CHARGER ALARM(OUT)", "UNKNOWN",
                                            "UNKNOWN", "UNKNOWN", "CHARGER ALARM(IN)", "UNKNOWN", "UNKNOWN",
                                            "UNKNOWN", "REGEN KEY ALARM", "UNKNOWN", "UNKNOWN", "UNKNOWN",
                                            "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN"
                                           };

static const char *pmu_power_off_reason[] = {"UNKNOWN", "POWERHOLD", "UNKNOWN", "UNKNOWN", "UNKNOWN",
                                             "UNKNOWN", "UNKNOWN", "UNKNOWN", "WDT CRST", "UNKNOWN",
                                             "LPSD", "UNKNOWN", "UNKNOWN", "SYSRSTB CRST", "CAP LPSD"
                                            };

#elif (defined(AB1568) || defined(MT2822))
static const char *pmu_power_on_reason[] = {"PWEKEY PRESS", "UNKNOWN","RTC ALARM", "UNKNOWN","CHARGER INSERTION IN EOC",
											"UNKNOWN","UNKNOWN","UNKNOWN","CHARGER OUT EOC","UNKNOWN",
											"UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN",
											"COLD RESET","UNKNOWN","UNKNOWN","UNKNOWN","CHARGER INSERTION(Cold Reset)",
											"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN"};
static const char *pmu_power_off_reason[] = {"NO PWR OFF", "PWRHOLD(RTC)", "UVLO", "THRDN", "UNKNOWN",
                                             "SW CRST", "UNKNOWN", "UNKNOWN", "WDT CRST", "UNKNOWN",
                                             "LPSD", "PUPSRC", "KEYPWR", "SYSRTSTB CRST", "CAP LPSD",
                                             "VCORE OC", "VIO18 OC", "VAUD18 OC", "VRF OC", "VCORE PG",
                                             "VIO18 PG", "VAUD18 PG", "VRF PG", "VA18 PG", "VLDO33 PG", "VSRAM PG", "VRF LFO PG"
                                            };
#endif

#if (defined(AB156X) || defined(MT2822))
static const char *rtc_power_reason[] = {"RTC_POWERED_REASON_UNKNOW",
                                         "RTC_POWERED_BY_1ST",
                                         "RTC_POWERED_BY_ALARM",
                                         "RTC_POWERED_BY_TICK",
                                         "RTC_POWERED_BY_EINT0",
                                         "RTC_POWERED_BY_EINT1",
                                         "RTC_POWERED_BY_EINT2",
                                         "RTC_POWERED_BY_CAPTOUCH",
                                         "RTC_POWERED_BY_PWRKEY"
                                        };
#endif

static bootreason_reason_t bootreason_lastreason = BOOTREASON_MAX;

/* Private functions ---------------------------------------------------------*/
static bootreason_reason_t bootreason_get_boot_reason(void)
{
    uint8_t reason = 0x0;

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&reason,
                     1);

    /* Check if assert happens */
    if (reason & BOOTREASON_ASSERT_RESET_FLAG) {
        return BOOTREASON_ASSERT;
    }

    /* Check if other exception happens */
    if (reason & BOOTREASON_EXCEPTION_RESET_FLAG) {
		/* check if xoff  happens */
		if (reason & BOOTREASON_XOFF_RESET_FLAG){
			return BOOTREASON_XOFF_RESET;
		}else{
    	    return BOOTREASON_EXCEPTION;
		}
	}

    /* Check if WDT Software reset happens */
    if (reason & BOOTREASON_WDT_SW_RESET_FLAG) {
        return BOOTREASON_WATCHDOG_RESET;
    }

    /* Check if Soft reset happens */
    if (reason & BOOTREASON_SOFT_RESET_FLAG) {
        return BOOTREASON_SOFT_RESET;
    }

    /* check if WDT timeout reset happens */
    if (bootreason_check_wdt_timeout_reset() == BOOTREASON_STATUS_OK) {
        /* Check if WDT timeout reset happens in sleep flow */
        if (reason & BOOTREASON_SLEEP_ENTER_FLAG) {
            return BOOTREASON_SLEEPERROR;
        }

        /* other WDT tiemout */
        return BOOTREASON_WATCHDOG;
    }

    /* Check the last power off reason */
    if (bootreason_check_normal_power_on() == BOOTREASON_STATUS_OK) {
        return BOOTREASON_NORMAL;
    }

    return BOOTREASON_UNKNOWN;
}

static void bootreason_flags_clear(void)
{
    char flag = BOOTREASON_INIT_FLAG;

    /* clear bootreason flags */
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET,
                     (const char *)&flag,
                     1);

    /* clear reserved bootreason flags */
    flag = 0x0;
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET + 1,
                     (const char *)&flag,
                     1);
}


/* Public functions ----------------------------------------------------------*/
void bootreason_set_flag_exception_reset(void)
{
    char flag;

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&flag,
                     1);

    flag = flag | BOOTREASON_EXCEPTION_RESET_FLAG;

    /* update bootreason */
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET,
                     (const char *)&flag,
                     1);
}

void bootreason_set_flag_assert_reset(void)
{
    char flag;

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&flag,
                     1);

    flag = flag | BOOTREASON_ASSERT_RESET_FLAG;

    /* update bootreason */
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET,
                     (const char *)&flag,
                     1);
}

void bootreason_set_flag_wdt_sw_reset(void)
{
    char flag;

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&flag,
                     1);

    flag = flag | BOOTREASON_WDT_SW_RESET_FLAG;

    /* update bootreason */
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET,
                     (const char *)&flag,
                     1);
}

void bootreason_set_flag_soft_reset(void)
{
    char flag;

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&flag,
                     1);

    flag = flag | BOOTREASON_SOFT_RESET_FLAG;

    /* update bootreason */
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET,
                     (const char *)&flag,
                     1);
}

void bootreason_set_flag_xoff_reset(void)
{
    char flag;

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&flag,
                     1);

    flag = flag | BOOTREASON_XOFF_RESET_FLAG;

    /* update bootreason */
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET,
                     (const char *)&flag,
                     1);
}


void bootreason_set_flag_enter_sleep(void)
{
    char flag;

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&flag,
                     1);

    flag = flag | BOOTREASON_SLEEP_ENTER_FLAG;

    /* update bootreason */
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET,
                     (const char *)&flag,
                     1);
}

void bootreason_clear_flag_exit_sleep(void)
{
    char flag;

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&flag,
                     1);

    flag = flag & (~(BOOTREASON_SLEEP_ENTER_FLAG));

    /* update bootreason */
    hal_rtc_set_data(BOOTREASON_FLAG_OFFSET,
                     (const char *)&flag,
                     1);
}

void bootreason_init(void)
{
    char flag[BOOTREASON_FLAG_BYTES] = {0};

#if defined(AB156X) || defined(MT2822)
    uint32_t rtc_pwr_reason = 0;
    uint32_t power_on_reason = 0;
    uint32_t power_off_reason = 0;

    extern uint8_t pmu_get_power_on_reason(void);
    power_on_reason = pmu_get_power_on_reason();
    extern uint8_t pmu_get_power_off_reason(void);
    power_off_reason = pmu_get_power_off_reason();
#endif

#if defined(AB1565)
    if (power_off_reason < 15) {
        printf("[PMU Power Off Reason]:%d,%s\r\n",  (unsigned int )power_off_reason, pmu_power_off_reason[power_off_reason]);
    } else {
        printf("[PMU Power Off Reason]:%d,UNKNOWN\r\n",  (unsigned int )power_off_reason);
    }

    /* rtc wake up ,bit 1 set*/
    if (power_on_reason & 0x2) {
        extern int hal_rtc_get_power_on_reason();
        rtc_pwr_reason = hal_rtc_get_power_on_reason();

        printf("RTC WAKE UP,RTC REASON]:%d,%s\r\n",  (unsigned int )rtc_pwr_reason, rtc_power_reason[rtc_pwr_reason]);
    } else {
        /* not rtc wake up */
        printf("[PMU Power On Reason]:%d,%s\r\n", (unsigned int )power_on_reason, pmu_power_on_reason[power_on_reason]);
    }
#elif (defined(AB1568) || defined(MT2822))

    printf("[PMU Power Off Reason]:%d,%s\r\n",  (unsigned int )power_off_reason, pmu_power_off_reason[power_off_reason]);

    /* rtc wake up ,bit 1 set */
    if (power_on_reason & 0x2) {
        extern int hal_rtc_get_power_on_reason();
        rtc_pwr_reason = hal_rtc_get_power_on_reason();

        printf("RTC WAKE UP,RTC REASON]:%d,%s\r\n",  (unsigned int )rtc_pwr_reason, rtc_power_reason[rtc_pwr_reason]);
    } else {
        /* not rtc wake up */
        printf("[PMU Power On Reason]:%d,%s\r\n",  (unsigned int )power_on_reason, pmu_power_on_reason[power_on_reason]);
    }
#else

#endif

    /* get bootreason */
    bootreason_lastreason = bootreason_get_boot_reason();

    /* get bootreason first byte */
    hal_rtc_get_data(BOOTREASON_FLAG_OFFSET,
                     (char *)&flag,
                     BOOTREASON_FLAG_BYTES);

    /* reset bootreason flags */
    bootreason_flags_clear();

    printf("[Boot Reason]:%s\r\n", bootreason_string[bootreason_lastreason]);
    log_hal_msgid_info("[Boot Reason Flag]:0x%x,0x%x\r\n", 2, flag[1], flag[0]);
}

bootreason_status_t bootreason_get_reason(bootreason_reason_t *reason)
{
    if (bootreason_lastreason < BOOTREASON_MAX) {
        /* bootreason_init has been called */
        *reason = bootreason_lastreason;
    } else {
        /* bootreason_init has not been called */
        *reason = bootreason_get_boot_reason();
    }

    return BOOTREASON_STATUS_OK;
}

bootreason_status_t bootreason_get_info(bootreason_info_t *info)
{
    bootreason_status_t ret = BOOTREASON_STATUS_OK;
    bootreason_reason_t lastreason;
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    uint32_t dump_index = 0;
    uint32_t dump_addr, dump_len = 0;
#endif /* EXCEPTION_MEMDUMP_MODE */

    /* info init */
    info->panic_file.data   = NULL;
    info->panic_file.len    = 0;
    info->registers.data    = NULL;
    info->registers.len     = 0;
    info->stack.data        = NULL;
    info->stack.len         = 0;
    info->custom.data       = NULL;
    info->custom.len        = 0;

    /* get boot reason */
    bootreason_get_reason(&lastreason);

    /* check boot reason */
    switch(lastreason)
    {
        case BOOTREASON_UNKNOWN:
        case BOOTREASON_NORMAL:
        case BOOTREASON_ASSERT:
        case BOOTREASON_EXCEPTION:
        case BOOTREASON_WATCHDOG:
        case BOOTREASON_SLEEPERROR:
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
            if(EXCEPTION_STATUS_OK != exception_minidump_region_query_latest_index(&dump_index))
            {
            	printf("exception_minidump_region_query_latest_index error !!!\r\n");
                ret = BOOTREASON_STATUS_ERROR;
                break;
            }

            if(EXCEPTION_STATUS_OK != exception_minidump_region_query_info(dump_index,
                                                                           &dump_addr,
                                                                           &dump_len))
            {
            	printf("exception_minidump_region_query_info error !!!\r\n");
                ret = BOOTREASON_STATUS_ERROR;
                break;
            }

            info->custom.data = (uint8_t *)dump_addr;
            info->custom.len  = dump_len;
			break;
#endif /* EXCEPTION_MEMDUMP_MODE */
        default:
            ret = BOOTREASON_STATUS_ERROR;
            break;
    }

    /* update info */
    info->reason = lastreason;

    return ret;
}

#endif /* MTK_BOOTREASON_CHECK_ENABLE */
