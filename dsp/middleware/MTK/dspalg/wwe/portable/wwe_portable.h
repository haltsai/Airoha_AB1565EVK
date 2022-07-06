/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#ifndef  __WWE_PORTABLE_H__
#define  __WWE_PORTABLE_H__

#ifdef MTK_WWE_USE_PIC

extern void *wwe_export_parameters[];

#ifdef MTK_WWE_AMA_USE_PIC
#include "pryon_lite_PRL1000.h"

typedef PryonLiteStatus(*PryonLite_GetConfigAttributes_t)(const PryonLiteV2Config *config, PryonLiteV2EventConfig *eventConfig, PryonLiteV2ConfigAttributes *configAttributes);
typedef PryonLiteStatus(*PryonLite_Initialize_t)(const PryonLiteV2Config *config, PryonLiteV2Handle *handle, PryonLiteEvent_Callback publicEventCallback, PryonLiteV2EventConfig *eventConfig, char *engineMem, size_t sizeofEngineMem);
typedef PryonLiteStatus(*PryonLite_Destroy_t)(PryonLiteV2Handle *handle);
typedef PryonLiteStatus(*PryonLite_PushAudioSamples_t)(PryonLiteV2Handle *handle, const short *samples, int sampleCount);
typedef int (*PryonLite_IsInitialized_t)(PryonLiteV2Handle *handle);
typedef PryonLiteStatus(*PryonLiteWakeword_SetDetectionThreshold_t)(PryonLiteWakewordHandle handle, const char *keyword, int detectThreshold);


#define PryonLite_GetConfigAttributes ((PryonLite_GetConfigAttributes_t)wwe_export_parameters[0])
#define PryonLite_Initialize ((PryonLite_Initialize_t)wwe_export_parameters[1])
#define PryonLite_Destroy ((PryonLite_Destroy_t)wwe_export_parameters[2])
#define PryonLite_PushAudioSamples ((PryonLite_PushAudioSamples_t)wwe_export_parameters[3])
#define PryonLite_IsInitialized ((PryonLite_IsInitialized_t)wwe_export_parameters[4])
#define PryonLiteWakeword_SetDetectionThreshold ((PryonLiteWakeword_SetDetectionThreshold_t)wwe_export_parameters[5])

#endif

#ifdef MTK_WWE_GSOUND_USE_PIC

typedef void *(*GoogleHotwordDspMultiBankInit_t)(void **memory_banks, int number_of_banks);
typedef int (*GoogleHotwordDspMultiBankProcess_t)(const void *samples, int num_samples, int *preamble_length_ms, void *memory_handle);
typedef void (*GoogleHotwordDspMultiBankReset_t)(void *memory_handle);
typedef int (*GoogleHotwordDspMultiBankGetMaximumAudioPreambleMs_t)(void *memory_handle);
typedef int (*GoogleHotwordVersion_t)(void);


#define GoogleHotwordDspMultiBankInit  ((GoogleHotwordDspMultiBankInit_t)wwe_export_parameters[0])
#define GoogleHotwordDspMultiBankProcess  ((GoogleHotwordDspMultiBankProcess_t)wwe_export_parameters[1])
#define GoogleHotwordDspMultiBankReset  ((GoogleHotwordDspMultiBankReset_t)wwe_export_parameters[2])
#define GoogleHotwordDspMultiBankGetMaximumAudioPreambleMs  ((GoogleHotwordDspMultiBankGetMaximumAudioPreambleMs_t)wwe_export_parameters[3])
#define GoogleHotwordVersion  ((GoogleHotwordVersion_t)wwe_export_parameters[4])
#define kGoogleHotwordRequiredDataAlignment (*(int *)wwe_export_parameters[5])

#endif

#endif

#endif // #ifndef  __WWE_PORTABLE_H__
