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
 * File: task_def.h
 *
 * Description: This file defines the informations of tasks.
 *
 */

#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOSConfig.h"

typedef enum {
    TASK_PRIORITY_IDLE = 0,                                 /**< Lowest, special for idle task. */
    TASK_PRIORITY_SYSLOG,                                   /**< special for syslog task. */

    /* User task priority begin, please define your task priority at this interval. */
    TASK_PRIORITY_LOW,                                      /**< Low. */
    TASK_PRIORITY_BELOW_NORMAL,                             /**< Below normal. */
    TASK_PRIORITY_NORMAL,                                   /**< Normal. */
    TASK_PRIORITY_ABOVE_NORMAL,                             /**< Above normal. */
    TASK_PRIORITY_HIGH,                                     /**< High. */
    TASK_PRIORITY_SOFT_REALTIME,                            /**< Soft real time. */
    TASK_PRIORITY_HARD_REALTIME,                            /**< Hard real time. */
    /* User task priority end. */

    /* Be careful, the max-priority number can not be bigger than configMAX_PRIORITIES - 1, or kernel will crash!!! */
    TASK_PRIORITY_TIMER = configMAX_PRIORITIES - 1,         /**< Highest, special for timer task to keep time accuracy. */
    TASK_PRIORITY_BT_ROUTINE_TASK = configMAX_PRIORITIES - 3,
    TASK_PRIORITY_BT_CMD_TASK = configMAX_PRIORITIES - 2,
} task_priority_type_t;

/* Part_1: SDK tasks configure infomation, please don't modify. */

/* The following is an example to define the XXXX task.
#define XXXX_TASK_NAME "XXXX"
#define XXXX_TASK_STACKSIZE 512
#define XXXX_TASK_PRIO TASK_PRIORITY_LOW
#define XXXX_QUEUE_LENGTH  16
*/

/* For os utilization task. */
#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
#define MTK_OS_CPU_UTILIZATION_TASK_NAME "CPU"
#define MTK_OS_CPU_UTILIZATION_STACKSIZE 2048
#define MTK_OS_CPU_UTILIZATION_PRIO      TASK_PRIORITY_SOFT_REALTIME
#endif

#ifdef MTK_USB_DEMO_ENABLED
/* USB. */
#define USB_TASK_NAME "USB"
#define USB_TASK_STACKSIZE 2048
#define USB_TASK_PRIO TASK_PRIORITY_HIGH
#define USB_QUEUE_LENGTH  50
#endif

/* For key event task. */
#define SCT_KEY_TASK_NAME               "sct_key"
#define SCT_KEY_TASK_STACKSIZE          (3000)
#define SCT_KEY_TASK_PRIO               TASK_PRIORITY_NORMAL
#define SCT_KEY_QUEUE_LENGTH            10

/* ATCI task definition. */
#define ATCI_TASK_NAME              "ATCI"
/* This definition determines whether the demo uses ATCI via Port Service port, such as UART/USB port. If it is not defined, then the demo accesses the ATCI via UART port only. */
#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
/* This definition determines whether the demo uses audio tuning functions, such as tuning audio performace or tuning gains. If it is not defined, then the demo does not using audio tuning functions. */
#ifdef MTK_AUDIO_TUNING_ENABLED
#define ATCI_TASK_STACKSIZE (1506) /* Unit StackType_t (u32). */
#else
#define ATCI_TASK_STACKSIZE (768) /* Unit StackType_t (u32). */
#endif

#elif defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
#ifdef MTK_AUDIO_TUNING_ENABLED
#define ATCI_TASK_STACKSIZE (1506) /* Unit StackType_t (u32). */
#else
#define ATCI_TASK_STACKSIZE (768) /* Unit StackType_t (u32). */
#endif

#else
/* This definition determines whether the demo uses audio tuning functions, such as tuning audio performace or tuning gains. If it is not defined, then the demo does not using audio tuning functions. */
#ifdef MTK_AUDIO_TUNING_ENABLED
#define ATCI_TASK_STACKSIZE         (1250) /* Unit StackType_t (u32). */
#else
#define ATCI_TASK_STACKSIZE         (384) /* Unit StackType_t (u32). */
#endif
#endif
#define ATCI_TASK_PRIO              TASK_PRIORITY_ABOVE_NORMAL

/* RACE task definition. */
#define RACE_TASK_NAME              ("race command")
#ifdef MTK_AUDIO_TUNING_ENABLED
#define RACE_TASK_STACKSIZE         (1506) /* Unit StackType_t (u32). */
#else
#define RACE_TASK_STACKSIZE         (512) /* Unit StackType_t (u32). */
#endif
#define RACE_TASK_PRIO              (TASK_PRIORITY_NORMAL)

/* BT task definition. */
#define BT_TASK_NAME              "bt_task"
#ifdef MTK_AMA_ENABLE
#define BT_TASK_STACKSIZE         (1064)         /* Unit StackType_t (u32). 4K, for AMA running in BT task*/
#else /* MTK_AMA_ENABLE */
#ifdef MTK_IAP2_PROFILE_ENABLE
#define BT_TASK_STACKSIZE         (1064)         /* Unit StackType_t (u32). */
#else /* MTK_IAP2_PROFILE_ENABLE */
#define BT_TASK_STACKSIZE         (1064)         /* Unit StackType_t (u32). */
#endif /* MTK_IAP2_PROFILE_ENABLE */
#endif /* MTK_AMA_ENABLE */
#define BT_TASK_PRIORITY          TASK_PRIORITY_HIGH

/* BT sink app task definition. */
#define BT_SINK_APP_TASK_NAME      "BT_sink_task"
#define BT_SINK_APP_TASK_STACKSIZE (640)        /* Unit StackType_t (u32). */
#define BT_SINK_APP_TASK_PRIO      TASK_PRIORITY_NORMAL

/* Audio manager task definition. */
#define AM_TASK_NAME             "AudioManager"
#define AM_TASK_STACKSIZE        (704)          /* Unit StackType_t (u32). */
#define AM_TASK_PRIO             TASK_PRIORITY_SOFT_REALTIME

/* App UI realtime task definition. */
#define UI_REALTIME_TASK_NAME             "UI_realtime"
#define UI_REALTIME_TASK_STACKSIZE        (375)          /*unit StackType_t (u32)*/
#define UI_REALTIME_TASK_PRIO             TASK_PRIORITY_SOFT_REALTIME


/* BATTERY CHARGER. */
#define BMT_TASK_NAME "BMT"
#define BMT_TASK_STACKSIZE 1192
#define BMT_TASK_PRIO TASK_PRIORITY_ABOVE_NORMAL
#define BMT_QUEUE_LENGTH  1

#define BMT_APP_TASK_NAME "battery_message_task"
#define BMT_APP_TASK_STACKSIZE 1400
#define BMT_APP_TASK_PRIO TASK_PRIORITY_BELOW_NORMAL
#define BMT_APP_QUEUE_LENGTH  5

#define BMT_TASK_TIMER_IN_100MS 100

/* For MP3 codec task */
#define MP3_CODEC_TASK_NAME       "mp3_codec_task"
#ifndef MTK_MP3_TASK_DEDICATE
#define MP3_CODEC_TASK_STACKSIZE  2048
#else
#define MP3_CODEC_TASK_STACKSIZE  384    /* For MP3 task dedicate in Task List. Size: 1.5k/sizeof(StackType_t). */
#endif
#define MP3_CODEC_TASK_PRIO       TASK_PRIORITY_SOFT_REALTIME
#define MP3_CODEC_QUEUE_LENGTH 20

/* For audio codec task. */
#define AUDIO_CODEC_TASK_NAME       "audio_codec_task"
#define AUDIO_CODEC_TASK_STACKSIZE  1024
#define AUDIO_CODEC_TASK_PRIO       TASK_PRIORITY_NORMAL
#define AUDIO_CODEC_QUEUE_LENGTH    20

/* For ui shell handle task. */
#define UI_SHELL_TASK_NAME         "ui_shell_task"
#define UI_SHELL_TASK_STACKSIZE    (2048)
#define UI_SHELL_TASK_PRIO         TASK_PRIORITY_NORMAL

/* For controler test task. */
#define CONTROLER_TEST_NAME              "controler_test_task"
#define CONTROLER_TEST_STACKSIZE         (640)         /* Unit StackType_t (u32). */
#define CONTROLER_TEST_PRIORITY          TASK_PRIORITY_HIGH

/* For gsound port task. */
#if defined GSOUND_LIBRARY_ENABLE && defined GSOUND_TARGET_TASK_ENABLE
#define GSOUND_TARGET_TASK_NAME         "G_TAR_TASK"
#define GSOUND_TARGET_TASK_STACK_POOL   512
#define GSOUND_TARGET_TASK_PRIORITY     TASK_PRIORITY_NORMAL
#endif

#ifdef MTK_AMA_ENABLE
/**
 * @brief for AMA audio task
 */
#define AMA_TARGET_TASK_NAME         "A_AMA"
#define AMA_TARGET_TASK_STACK_SIZE   1024
#define AMA_TARGET_TASK_PRIORITY     TASK_PRIORITY_NORMAL

/**
 * @brief for AMA core internal task
 */
#define AMA_CORE_TASK_NAME          "C_AMA"
#define AMA_CORE_TASK_STACK_SIZE    512
#define AMA_CORE_TASK_PRIORITY      TASK_PRIORITY_NORMAL
#endif

/* Iap2 service task definition. */
#define IAP2_SRV_TASK_NAME              "iAP2_service"
#define IAP2_SRV_TASK_STACKSIZE         (1024*2) /* Unit byte. */
#define IAP2_SRV_TASK_PRIORITY          TASK_PRIORITY_NORMAL
#define IAP2_SRV_QUEUE_LENGTH           50

/* Part_2: Application and customer tasks configure information. */
/* Currently, only UI task and tasks to show example project function which define in apps/project/src/main.c. */

/**
 * @brief      This function is to initialize some tasks, such as ATCI.
 */
void task_def_init(void);

/**
 * @brief      This function is to create some tasks, such as ATCI.
 */
void task_def_create(void);

#ifdef __cplusplus
}
#endif
#endif

