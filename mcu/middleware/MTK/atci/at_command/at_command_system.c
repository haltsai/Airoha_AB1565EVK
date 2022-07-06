/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "FreeRTOS.h"
#include "task.h"

#include "at_command.h"
#include "syslog.h"
#include "task_def.h"
#include "os_port_callback.h"
#include "hal_wdt.h"
#include "hal_gpt.h"
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif
#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATSS: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATSS: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"ATSS: "fmt,##arg)
#define LOGMSGIDE(fmt,cnt,arg...)   LOG_MSGID_E(atcmd, "ATSS: "fmt,cnt,##arg)
#define LOGMSGIDW(fmt,cnt,arg...)   LOG_MSGID_W(atcmd ,"ATSS: "fmt,cnt,##arg)
#define LOGMSGIDI(fmt,cnt,arg...)   LOG_MSGID_I(atcmd ,"ATSS: "fmt,cnt,##arg)


/*--- Function ---*/
#if defined(MTK_SYSTEM_AT_COMMAND_ENABLE)
extern atci_status_t atci_cmd_hdlr_system(atci_parse_cmd_param_t *parse_cmd);
#endif /* MTK_SYSTEM_AT_COMMAND_ENABLE */
#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
extern atci_status_t atci_cmd_hdlr_utilization(atci_parse_cmd_param_t *parse_cmd);
#endif /* MTK_OS_CPU_UTILIZATION_ENABLE */
#if defined(MTK_SWLA_ENABLE)
extern atci_status_t atci_cmd_hdlr_swla(atci_parse_cmd_param_t *parse_cmd);
#endif /* MTK_SWLA_ENABLE */
/** <pre>
* <b>[Category]</b>
*    System Service
* <b>[Description]</b>
*    Show system information and trigger system reboot or crash
* <b>[Command]</b>
*    AT+SYSTEM=<module>
* <b>[Parameter]</b>
*    <module> string type
*    task: show all freeRTOS tasks information
*    mem: show heap status
*    crash: trigger system crash to dump system infomation
*    reboot: trigger system to hard-reboot
* <b>[Response]</b>
*    OK with data or ERROR;
* <b>[Example]</b>
* @code
*    Send:
*        AT+SYSTEM=task
*    Response:
*        +SYSTEM:
*        parameter meaning:
*        1: pcTaskName
*        2: cStatus(Ready/Blocked/Suspended/Deleted)
*        3: uxCurrentPriority
*        4: usStackHighWaterMark (unit is sizeof(StackType_t))
*        5: xTaskNumber
*        OK
*        +SYSTEM:
*        ATCI   X   5   594 2
*        IDLE   R   0   223 5
*        Tmr S  B   14  326 6
*        OK
* @endcode
* <b>[Note]</b>
*    MTK_SYSTEM_AT_COMMAND_ENABLE should be defined in y in project's feature.mk
* </pre>
*/

#if ( ( ( defined(MTK_SYSTEM_AT_COMMAND_ENABLE) ) && ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) ) || defined(MTK_OS_CPU_UTILIZATION_ENABLE) )
static void construct_profiling_info(const char *prefix, atci_response_t *presponse, char *task_list_buf)
{
    char *substr = NULL, *saveptr = NULL;
    int32_t pos = 0;

    LOGI("##task_list_buf: \r\n%s\r\n", task_list_buf);
    substr = strtok_r(task_list_buf, "\n", &saveptr);
    pos = snprintf((char *)(presponse->response_buf), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "%s", prefix);
    pos += snprintf((char *)(presponse->response_buf + pos), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "%s\n", substr);
    while (substr) {
        /* LOGI("##substr: \r\n%s##\r\n",substr); */
        substr = strtok_r(NULL, "\n", &saveptr);
        if (substr == NULL) {
            /* on MT2625, 0x0 is protected by mpu with XN attribute, so cannot touch 0x0, even read operation */
            break;
        } else if ((pos + strlen(substr)) <= ATCI_UART_TX_FIFO_BUFFER_SIZE) {
            pos += snprintf((char *)(presponse->response_buf + pos),
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                            "%s\n",
                            substr);
            /*LOGI("##buf: \r\n%s##\r\n",(char *)(presponse->response_buf)); */
        } else {
            /*LOGI("##buf: \r\n%s##\r\n",(char *)(presponse->response_buf)); */
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
            atci_send_response(presponse);
            pos = 0;
            pos += snprintf((char *)(presponse->response_buf + pos),
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                            "%s",
                            prefix);
            pos += snprintf((char *)(presponse->response_buf + pos),
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                            "%s\n",
                            substr);
        }
    }
    if (strlen((char *)(presponse->response_buf)) != 0) {
        /*LOGI("##buf: \r\n%s##\r\n",(char *)(presponse->response_buf)); */
        presponse->response_len = strlen((char *)(presponse->response_buf));
        presponse->response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
        atci_send_response(presponse);
    }
}
#endif /* if ( ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) ) || defined(MTK_OS_CPU_UTILIZATION_ENABLE) ) */

static void stringToLower(char *pString)
{
    uint16_t index;
    uint16_t length = (uint16_t)strlen((char *)pString);

    for (index = 0; index <= length; index++) {
        pString[index] = (char)tolower((unsigned char)pString[index]);
    }
}

#if defined(MTK_SYSTEM_AT_COMMAND_ENABLE)

static void system_show_usage(uint8_t *buf)
{
    int32_t pos = 0;

    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+SYSTEM:\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(AT+SYSTEM=<module>\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(<module>=task|mem|crash|reboot)\r\n");
}

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
static void system_show_task_info(atci_response_t *presponse)
{
    char *task_list_buf;
    int32_t buf_size = uxTaskGetNumberOfTasks() * (configMAX_TASK_NAME_LEN + 18);

    if ((task_list_buf = pvPortMalloc(buf_size)) == NULL) {
        LOGMSGIDE("memory malloced failed.\n", 0);
        strncpy((char *)(presponse->response_buf),
                "+SYSTEM:\r\nheap memory is not enough to do task information profiling\r\n",
                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
        presponse->response_len = strlen((char *)(presponse->response_buf));
        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(presponse);
        return;
    }

    strncpy((char *)(presponse->response_buf),
            "+SYSTEM:\r\nparameter meaning:\r\n1: pcTaskName\r\n2: cStatus(Ready/Blocked/Suspended/Deleted)\r\n3: uxCurrentPriority\r\n4: usStackHighWaterMark(unit is sizeof(StackType_t))\r\n5: xTaskNumber\r\n",
            (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
    presponse->response_len = strlen((char *)(presponse->response_buf));
    presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
    atci_send_response(presponse);

    vTaskList(task_list_buf);
    construct_profiling_info("+SYSTEM:\r\n", presponse, task_list_buf);

    vPortFree(task_list_buf);
}
#endif

static void system_query_mem(uint8_t *buf)
{
    int32_t pos = 0;
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+SYSTEM:\r\nheap information: \r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "\ttotal: %d\r\n",
                    configTOTAL_HEAP_SIZE);
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "\tcurrent free: %d\r\n",
                    xPortGetFreeHeapSize());
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "\tmini free: %d\r\n",
                    xPortGetMinimumEverFreeHeapSize());
}

/* AT command handler  */
atci_status_t atci_cmd_hdlr_system(atci_parse_cmd_param_t *parse_cmd)
{
    char *param = NULL, *saveptr = NULL;
    atci_response_t *presponse = pvPortMalloc(sizeof(atci_response_t));
    if (presponse == NULL) {
        LOGMSGIDE("memory malloced failed.\r\n", 0);
        return ATCI_STATUS_ERROR;
    }

    memset(presponse, 0, sizeof(atci_response_t));
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    presponse->cmd_id = parse_cmd->cmd_id;
#endif /* ATCI_APB_PROXY_ADAPTER_ENABLE */

    /* To support both of uppercase and lowercase */
    stringToLower(parse_cmd->string_ptr);

    presponse->response_flag = 0; /* Command Execute Finish. */
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_READ: /* rec: AT+SYSTEM? */
        case ATCI_CMD_MODE_TESTING: /* rec: AT+SYSTEM=? */
            system_show_usage(presponse->response_buf);
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(presponse);
            break;
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+SYSTEM=<module> */
            param = parse_cmd->string_ptr + parse_cmd->parse_pos;
            param = strtok_r(param, "\n\r", &saveptr);
            LOGI("##module=%s##\r\n", param);

            if (strcmp(param, "task") == 0) {
#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
                system_show_task_info(presponse);
#else
                strncpy((char *)(presponse->response_buf),
                        "+SYSTEM:\r\nplease define configUSE_TRACE_FACILITY = 1 and configUSE_STATS_FORMATTING_FUNCTIONS = 1 in project freeRTOSConfig.h \r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(presponse);
#endif
            } else if (strcmp(param, "mem") == 0) {
                system_query_mem(presponse->response_buf);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(presponse);
            } else if (strcmp(param, "crash") == 0) {
                strncpy((char *)(presponse->response_buf),
                        "+SYSTEM:\r\n system will crash...\r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(presponse);
                configASSERT(0);
            } else if (strcmp(param, "reboot") == 0) {
#if 0
                hal_wdt_config_t wdt_config;

                wdt_config.mode = HAL_WDT_MODE_RESET;
                wdt_config.seconds = 1;
                hal_wdt_init(&wdt_config);
                hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#else
                (void)hal_wdt_software_reset();
#endif
                presponse->response_len = 0;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                /* command syntax error */
                strncpy((char *)(presponse->response_buf),
                        "+SYSTEM:\r\ncommand syntax error\r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(presponse);
            }
            break;
        default :
            /* others are invalid command format */
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(presponse);
            break;
    }

    vPortFree(presponse);
    return ATCI_STATUS_OK;
}
#endif /* MTK_SYSTEM_AT_COMMAND_ENABLE */
/*
AT+UTILIZATION=?
+UTILIZATION:
AT+UTILIZATION=<op> [,<tick,count>])
<op> string type
start: begin os profiling
stop: stop os profiling
duration: enable os profiling with in a period of time
[,<tick,count>] integer type
<tick> integer type, means the profiling duration, the unit is system tick, 1tick is 1ms, only needed when <op>=duration
<count> integer type, means the profiling counts, only needed when <op>=duration
*/

/* <pre>
* <b>[Category]</b>
*    System Service
* <b>[Description]</b>
*    Profile cpu utilization
* <b>[Command]</b>
*    AT+UTILIZATION=<op> [,<tick,count>])
* <b>[Parameter]</b>
*    <op> string type, must be 'start', 'stop' or 'duration'
*    <tick> integer type, means the profiling duration, the unit is system tick, 1tick is 1ms, only needed when <op>=duration
*    <count> integer type, means the profiling counts, only needed when <op>=duration
* <b>[Response]</b>
*    OK with profling data or ERROR;
* <b>[Example]</b>
* @code
*    Send:
*        AT+UTILIZATION= duration,5000,10
*    Response:
*        +UTILIZATION:
*        profiling begin, duration is 5000 tick, the tick's unit is 1/configTICK_RATE_HZ
*        OK
*        +UTILIZATION:
*        parameter meaning:
*        1: pcTaskName
*        2: count(unit is 32k of gpt)
*        3: ratio
*        +UTILIZATION:
*        CPU    0       <1%
*        IDLE   163943  99%
*        Tmr S  89      <1%
*        ATCI   19      <1%
* @endcode
* <b>[Note]</b>
*    MTK_OS_CPU_UTILIZATION_ENABLE should be defined in y in project's feature.mk
* </pre>
*/
#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
typedef enum {
    CPU_MEASURE_NOT_START = 0,
    CPU_MEASURE_START_MODE = 1,
    CPU_MEASURE_DURATION_MODE = 2,
} _cpu_measure_state_t;

static void utilization_show_usage(uint8_t *buf)
{
    int32_t pos = 0;

    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+UTILIZATION:\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(AT+UTILIZATION=<op> [,<tick,count>])\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(<op>=[start|stop|duration]\r\n<tick> means the profiling duration, the unit is system tick. <count> means the profiling counts, <tick> and <count> are only needed when <op>=duration)\r\n");
}

static TaskHandle_t xUtilTaskHandle = NULL;
static uint32_t _cpu_meausre_time;
static uint32_t _cpu_meausre_counts;
static uint32_t _cpu_meausre_mode = CPU_MEASURE_NOT_START;

uint32_t get_count_sleep_time_us(void);
uint32_t get_count_idle_time_us(void);
void tickless_start_count_idle_ratio(void);
void tickless_stop_count_idle_ratio(void);

void _cpu_utilization_task(void *arg)
{
    char *task_list_buf = NULL;
    atci_response_t *presponse = NULL;
    int32_t buf_size = uxTaskGetNumberOfTasks() * (configMAX_TASK_NAME_LEN + 20);
    TickType_t xLastWakeTime;
#if (configUSE_TICKLESS_IDLE == 2) && (PRODUCT_VERSION == 2822 || PRODUCT_VERSION == 1568 || PRODUCT_VERSION == 1565)
    uint32_t pos = 0;
    uint32_t start_time = 0;
    uint32_t stop_time = 0;
    float duration = 0;
    float sleep_percentage = 0;
    float idle_percentage = 0;
    float busy_percentage = 0;
#endif

#if 0
    UBaseType_t stackmark = MTK_OS_CPU_UTILIZATION_STACKSIZE - uxTaskGetStackHighWaterMark(NULL) * 4;
    LOGMSGIDI("utilization task stack usage: %d, 0x%x \r\n", 2, stackmark, uxTaskGetBottomOfStack(NULL));
#endif
    if ((presponse = pvPortMalloc(sizeof(atci_response_t))) != NULL) {
        memset(presponse, 0, sizeof(atci_response_t));

        if ((task_list_buf = pvPortMalloc(buf_size)) == NULL) {
            LOGMSGIDE("memory malloced failed, and do cpu utilization \r\n", 0);
            strncpy((char *)(presponse->response_buf),
                    "+UTILIZATION:\r\nheap memory is not enough to do cpu utilization\r\n Error\r\n",
                    ATCI_UART_TX_FIFO_BUFFER_SIZE);
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
            atci_send_response(presponse);
        } else {
            while (_cpu_meausre_counts) {
                /* none risk condition here, because utilization task priority is always higher than atcmd task */
                _cpu_meausre_counts--;

                xLastWakeTime = xTaskGetTickCount();
                vConfigureTimerForRunTimeStats();
                vTaskClearTaskRunTimeCounter();

                if (_cpu_meausre_time > 0) {
                    //vTaskDelay(_cpu_meausre_time); /* duration mode */
                    vTaskDelayUntil(&xLastWakeTime, _cpu_meausre_time);  /* duration mode */
                    if (!_cpu_meausre_counts) {
                        /* profiling is ended by at+utilization = stop */
                        break;
                    }
                } else {
#if (configUSE_TICKLESS_IDLE == 2) && (PRODUCT_VERSION == 2822 || PRODUCT_VERSION == 1568 || PRODUCT_VERSION == 1565)
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_time);
                    tickless_start_count_idle_ratio();
#endif
                    vTaskSuspend(NULL); /* start/stop mode */
                }

                /* after resume */
                vTaskGetRunTimeStats(task_list_buf);

                /* Print idle time ratio */
#if (configUSE_TICKLESS_IDLE == 2) && (PRODUCT_VERSION == 2822 || PRODUCT_VERSION == 1568 || PRODUCT_VERSION == 1565)
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &stop_time);
                duration = ((unsigned int)(((double)(stop_time - start_time)) / 32.768)) / ((1000 / configTICK_RATE_HZ));
                idle_percentage = (float)get_count_idle_time_us() / 1000 / duration;
                sleep_percentage = (float)get_count_sleep_time_us() / 1000 / duration;
                busy_percentage = 1 - idle_percentage - sleep_percentage;
                tickless_stop_count_idle_ratio();

                pos += snprintf((char *)(presponse->response_buf + pos), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "[CM4 IDLE TIME]Duration: %lu\r\n", (uint32_t)duration);
                pos += snprintf((char *)(presponse->response_buf + pos), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "[CM4 IDLE TIME]Idle_time: %lu ms\r\n", (uint32_t)get_count_idle_time_us()/1000);
                pos += snprintf((char *)(presponse->response_buf + pos), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "[CM4 IDLE TIME]Sleep_time: %lu ms\r\n", (uint32_t)get_count_sleep_time_us()/1000);
                pos += snprintf((char *)(presponse->response_buf + pos), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "[CM4 IDLE TIME]Idle time percentage: [%lu]\r\n", (uint32_t)(idle_percentage * 100));
                pos += snprintf((char *)(presponse->response_buf + pos), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "[CM4 IDLE TIME]Sleep time percentage: [%lu]\r\n", (uint32_t)(sleep_percentage * 100));
                pos += snprintf((char *)(presponse->response_buf + pos), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "[CM4 IDLE TIME]Busy time percentage: [%lu]\r\n", (uint32_t)(busy_percentage * 100));
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(presponse);
#endif

                strncpy((char *)(presponse->response_buf),
                        "+UTILIZATION:\r\nparameter meaning:\r\n1: pcTaskName\r\n2: count(unit is 32k of gpt)\r\n3: ratio\r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);

                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(presponse);

                construct_profiling_info("+UTILIZATION:\r\n", presponse, task_list_buf);
            }

            vPortFree(task_list_buf);
            vPortFree(presponse);
#if 0
            stackmark = MTK_OS_CPU_UTILIZATION_STACKSIZE - uxTaskGetStackHighWaterMark(NULL) * 4;
            LOGMSGIDI("stack usage: %d \r\n", 1, stackmark);
#endif
            _cpu_meausre_mode = CPU_MEASURE_NOT_START;
            vTaskDelete(xTaskGetCurrentTaskHandle());
        }
    } else {
        LOGMSGIDE("memory malloced failed, and do cpu utilization \r\n", 0);
        vTaskDelete(xTaskGetCurrentTaskHandle());
    }
    while (1);
}

static BaseType_t utilization_cpu_benchmark(uint32_t duration, uint32_t count)
{
    BaseType_t ret = pdPASS;
    _cpu_meausre_time = duration;
    _cpu_meausre_counts = count;
    ret = xTaskCreate(_cpu_utilization_task,
                      MTK_OS_CPU_UTILIZATION_TASK_NAME,
                      MTK_OS_CPU_UTILIZATION_STACKSIZE / sizeof(StackType_t),
                      NULL,
                      MTK_OS_CPU_UTILIZATION_PRIO,
                      &xUtilTaskHandle);
    return ret;
}

atci_status_t atci_cmd_hdlr_utilization(atci_parse_cmd_param_t *parse_cmd)
{
    char *op = NULL, *param = NULL, *saveptr = NULL;
    BaseType_t ret;
    uint32_t tick = 0, counts = 0;
    atci_response_t *presponse = pvPortMalloc(sizeof(atci_response_t));
    if (presponse == NULL) {
        LOGMSGIDE("memory malloced failed.\r\n", 0);
        return ATCI_STATUS_ERROR;
    }
    memset(presponse, 0, sizeof(atci_response_t));
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    presponse->cmd_id = parse_cmd->cmd_id;
#endif /* ATCI_APB_PROXY_ADAPTER_ENABLE */

    /* To support both of uppercase and lowercase */
    stringToLower(parse_cmd->string_ptr);

    presponse->response_flag = 0; /* Command Execute Finish. */
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_READ: /* rec: AT+UTILIZATION?  */
        case ATCI_CMD_MODE_TESTING: /* rec: AT+UTILIZATION=? */
            utilization_show_usage(presponse->response_buf);
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(presponse);
            break;
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+UTILIZATION=<op> [,<tick,count>]) */
            op = parse_cmd->string_ptr + parse_cmd->parse_pos;
            op = strtok_r(op, "\n\r", &saveptr);
            op = strtok_r(op, ",", &saveptr);
            //LOGI("## op=%s ##\r\n", op);
            if (strcmp(op, "start") == 0) {
                /* start/stop mode */
                switch (_cpu_meausre_mode) {
                    case CPU_MEASURE_NOT_START:
                        _cpu_meausre_mode = CPU_MEASURE_START_MODE;
                        ret = utilization_cpu_benchmark(0, 1);
                        if (ret == pdPASS) {
                            strncpy((char *)(presponse->response_buf),
                                    "+UTILIZATION:\r\ncpu utilization profing begin, please use AT+UTILIZATION=stop to end profiling...\r\n",
                                    (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                            presponse->response_len = strlen((char *)(presponse->response_buf));
                            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                            atci_send_response(presponse);
                        } else {
                            LOGMSGIDE("memory malloced failed, and cannot create profiling task\r\n", 0);
                            strncpy((char *)(presponse->response_buf),
                                    "+UTILIZATION:\r\nheap memory is not enough to do cpu utilization\r\n",
                                    (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                            presponse->response_len = strlen((char *)(presponse->response_buf));
                            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                            atci_send_response(presponse);
                            _cpu_meausre_mode = CPU_MEASURE_NOT_START;
                        }
                        break;
                    case CPU_MEASURE_START_MODE:
                    case CPU_MEASURE_DURATION_MODE:
                        strncpy((char *)(presponse->response_buf),
                                "+UTILIZATION:\r\nutilization profiling already started, please use AT+UTILIZATION=stop to stop profiling\r\n",
                                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                        presponse->response_len = strlen((char *)(presponse->response_buf));
                        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        atci_send_response(presponse);
                        break;
                }
            } else if (strcmp(op, "duration") == 0) {
                /* duration mode */
                //LOGI("1## param=%s ##\r\n", saveptr);
                param = strtok_r(NULL, ",", &saveptr);
                if (param != NULL) {
                    //LOGI("2## param=%s ##\r\n", param);
                    tick = atoi(param);
                    param = strtok_r(NULL, ",", &saveptr);
                    if (param != NULL) {
                        //LOGI("3## param=%s ##\r\n", param);
                        counts = atoi(param);
                    }
                }

                switch (_cpu_meausre_mode) {
                    case CPU_MEASURE_NOT_START:
                        /* param validity check */
                        if ((tick <= 0) || (counts <= 0)) {
                            LOGMSGIDE("duration mode parameter invalid, tick=%d,counts=%d\r\n", 2, tick, counts);
                            strncpy((char *)(presponse->response_buf),
                                    "+UTILIZATION:\r\nparam must be positive integer if <op>=duration, which means profiling duration and profiling counts\r\n",
                                    (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                            presponse->response_len = strlen((char *)(presponse->response_buf));
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                            atci_send_response(presponse);
                        } else {
                            /* start profiling */
                            _cpu_meausre_mode = CPU_MEASURE_DURATION_MODE;
                            ret = utilization_cpu_benchmark(tick, counts); /* duration mode */
                            if (ret == pdPASS) {
                                snprintf((char *)(presponse->response_buf),
                                         ATCI_UART_TX_FIFO_BUFFER_SIZE,
                                         "+UTILIZATION:\r\n profiling begin, duration is %d tick, and profiling %d counts, the tick's unit is 1/configTICK_RATE_HZ\r\n",
                                         (int)tick, (int)counts);
                                presponse->response_len = strlen((char *)(presponse->response_buf));
                                presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                                atci_send_response(presponse);
                            } else {
                                LOGMSGIDE("memory malloced failed, and cannot create profiling task\r\n", 0);
                                strncpy((char *)(presponse->response_buf),
                                        "+UTILIZATION:\r\nheap memory is not enough to do cpu utilization\r\n",
                                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                                presponse->response_len = strlen((char *)(presponse->response_buf));
                                presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                                atci_send_response(presponse);
                                _cpu_meausre_mode = CPU_MEASURE_NOT_START;
                            }
                        }
                        break;
                    case CPU_MEASURE_START_MODE:
                    case CPU_MEASURE_DURATION_MODE:
                        /* profiling already begin by start mode, duration mode is invalid until profiling stop by AT+UTILIZATION=stop */
                        strncpy((char *)(presponse->response_buf),
                                "+UTILIZATION:\r\nprofiling already begin by AT+UTILIZATION=start, duration mode is invalid until profiling stoped by AT+UTILIZATION=stop\r\n",
                                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                        presponse->response_len = strlen((char *)(presponse->response_buf));
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        atci_send_response(presponse);
                        break;
                }
            } else if (strcmp(op, "stop") == 0) {
                switch (_cpu_meausre_mode) {
                    case CPU_MEASURE_NOT_START:
                        strncpy((char *)(presponse->response_buf),
                                "+UTILIZATION:\r\nutilization profiling didn't start, please use AT+UTILIZATION=start or AT+UTILIZATION= duration,tick,count to start firstly\r\n",
                                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                        presponse->response_len = strlen((char *)(presponse->response_buf));
                        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        atci_send_response(presponse);
                        break;
                    case CPU_MEASURE_START_MODE:
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                        presponse->response_len = 0;
                        atci_send_response(presponse);
                        vTaskResume(xUtilTaskHandle);
                        _cpu_meausre_mode = CPU_MEASURE_NOT_START;
                        break;
                    case CPU_MEASURE_DURATION_MODE:
                        /* set the measure count to zero to end the profiling */
                        _cpu_meausre_counts = 0;
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                        presponse->response_len = 0;
                        atci_send_response(presponse);
                        _cpu_meausre_mode = CPU_MEASURE_NOT_START;
                        break;
                }
            } else {
                /* command syntax error */
                strncpy((char *)(presponse->response_buf),
                        "+UTILIZATION:\r\n command syntax error\r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(presponse);
            }
            break;
        default :
            /* others are invalid command format */
            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            presponse->response_len = strlen((char *)(presponse->response_buf));
            atci_send_response(presponse);
            break;
    }

    vPortFree(presponse);
    return ATCI_STATUS_OK;
}
#endif /* MTK_OS_CPU_UTILIZATION_ENABLE */

/*
AT+SWLA=?
+SWLA:
AT+SWLA=<op>
<op> string type
enable: enable SWLA
disable: disable SWLA
*/

/* <pre>
* <b>[Category]</b>
*    System Service
* <b>[Description]</b>
*    Enable or disable SWLA at runtime
* <b>[Command]</b>
*    AT+SWLA=<op>
* <b>[Parameter]</b>
*    <op> string type, must be 'enable' or 'disable'
* <b>[Response]</b>
*    OK or ERROR;
* <b>[Example]</b>
* @code
*    Send:
*        AT+SWLA= enable
*    Response:
*        OK
* @endcode
* <b>[Note]</b>
*    MTK_SWLA_ENABLE should be defined in y in project's feature.mk
* </pre>
*/
#ifdef MTK_SWLA_ENABLE
static void swla_show_usage(uint8_t *buf)
{
    int32_t pos = 0;

    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+SWLA:\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(AT+SWLA=<op>)\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(<op>=[enable|disable]\r\n");
}
/* AT command handler  */
atci_status_t atci_cmd_hdlr_swla(atci_parse_cmd_param_t *parse_cmd)
{
    char *param = NULL, *saveptr = NULL;
    atci_response_t *presponse = pvPortMalloc(sizeof(atci_response_t));
    if (presponse == NULL) {
        LOGMSGIDE("memory malloced failed.\r\n", 0);
        return ATCI_STATUS_ERROR;
    }

    memset(presponse, 0, sizeof(atci_response_t));
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    presponse->cmd_id = parse_cmd->cmd_id;
#endif /* ATCI_APB_PROXY_ADAPTER_ENABLE */

    /* To support both of uppercase and lowercase */
    stringToLower(parse_cmd->string_ptr);

    presponse->response_flag = 0; /* Command Execute Finish. */
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_READ: /* rec: AT+SWLA? */
        case ATCI_CMD_MODE_TESTING: /* rec: AT+SWLA=? */
            swla_show_usage(presponse->response_buf);
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(presponse);
            break;
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+SWLA=<op> */
            param = parse_cmd->string_ptr + parse_cmd->parse_pos;
            param = strtok_r(param, "\n\r", &saveptr);
            LOGI("##op=%s##\r\n", param);

            if (strcmp(param, "enable") == 0) {
                /* swla enable, will re-start profiling. */
                SLA_Control(SA_ENABLE);
                presponse->response_len = 0;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strcmp(param, "disable") == 0) {
                /* swla disable, will freeze swla profiling. */
                SLA_Control(SA_DISABLE);
                presponse->response_len = 0;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                /* command syntax error */
                strncpy((char *)(presponse->response_buf),
                        "+SWLA:\r\ncommand syntax error\r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            atci_send_response(presponse);
            break;
        default :
            /* others are invalid command format */
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(presponse);
            break;
    }

    vPortFree(presponse);
    return ATCI_STATUS_OK;
}
#endif /* MTK_SWLA_ENABLE */ 