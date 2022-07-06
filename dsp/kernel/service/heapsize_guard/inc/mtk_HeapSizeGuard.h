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

#ifndef _MTK_HEAPSIZEGUARD_HEADER_
#define _MTK_HEAPSIZEGUARD_HEADER_

typedef struct HEAP_STATISTICS_INFO {
    TaskHandle_t xTaskHandle;
    char ucTaskName[configMAX_TASK_NAME_LEN];
    uint32_t xPeakAllocSize;             /* peak allocated heap size */
    uint32_t xCurAllocSize;              /* current allocated heap size */
    BaseType_t xHeapGuardFlag;          /* enable heap guard flag */
} HeapStatisticsInfo_t;

/**
 * Register heap size guard
**/
extern BaseType_t xHeapSizeGuard_Register(TaskHandle_t xTaskHandle);

/**
 * Start heap size guard
 * note1: history statistic data will be cleard
 * note2: if input NULL,  all tasks heap size guard are started
**/
extern BaseType_t xHeapSizeGuard_Restart(TaskHandle_t xTaskHandle);


/**
 * Stop heap size guard
 * note1: if input NULL, all tasks heap size guard are stoped
**/
extern BaseType_t xHeapSizeGuard_Stop(TaskHandle_t xTaskHandle);

/**
 * Get heap size guard statistic raw data
 * note1: if input NULL, all tasks heap size guard statistic raw data are dumped
**/
extern HeapStatisticsInfo_t *pxHeapSizeGuard_GetRawInfo(TaskHandle_t xTaskHandle);
/**
 * Dump heap size guard statistic information
 * note1: if input NULL, all tasks heap size guard statistic info are dumped
**/
extern void vHeapSizeGuard_DumpInfo(TaskHandle_t xTaskHandle);

/**
 * Dump system initialization stage heap size usage information
**/
extern void vHeapSizeGuard_DumpSysInitInfo(void);


#endif /*_MTK_HEAPSIZEGUARD_HEADER_*/
