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

#ifdef MTK_USB_DEMO_ENABLED

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "task_def.h"

/* USB Middleware includes */
#include "usb.h"
#include "usb_comm.h"
#include "usb_case.h"
#include "usb_custom.h"
#include "usb_main.h"
#include "usb_resource.h"

#include "usb_host_detect.h"
#include "usbacm_drv.h"
#include "usbacm_adap.h"
#include "usbms_drv.h"
#include "usbms_adap.h"
#include "usbhid_drv.h"
#include "usbhid_adap.h"
#include "usbaudio_drv.h"

#ifdef MTK_USB_XBOX_ENABLE
#include "usb_xbox.h"
#endif

/* Other Middleware includes */
#include "nvkey.h"
#include "nvkey_id_list.h"

/* Hal includes */
#include "hal_cache.h"

#include "hal_gpio.h"
#include "hal_pmu.h"
#include "hal_usb_internal.h"
#include "hal_nvic_internal.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_platform.h"
#include "memory_attribute.h"

/* Syslog create module for usb debug */
#include "syslog.h"
log_create_module_variant(USB_DEBUG, DEBUG_LOG_OFF, PRINT_LEVEL_INFO);

/* Syslog create module for usb_main.c */
log_create_module_variant(USB_MAIN, DEBUG_LOG_ON, PRINT_LEVEL_INFO);

#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
#include "battery_management.h"
#endif
#endif

#if defined(AB1568)
#include "hal_pmu_charger_2568.h"
#endif

#if defined(AB1565)
#include "hal_pmu_charger_2565.h"
#endif

#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
#include "battery_management_bc12.h"
#endif
#endif
#ifdef  __USB_MASS_STORAGE_ENABLE__
#include "usbms_state.h"
#endif

#ifdef MTK_PORT_SERVICE_ENABLE
#include "serial_port.h"
extern serial_port_register_callback_t g_serial_port_usb_callback[MAX_USB_PORT_NUM];
#endif

#ifdef  __USB_MASS_STORAGE_ENABLE__
msc_register_callback_t g_msc_usb_callback = NULL;
#endif

#ifdef USB_DETECT_LED
#include "hal.h"
#include "hal_pmu.h"
#include "hal_platform.h"
#endif

#define BOOT_TIMEOUT_CNT 900000
#define BOOT_ENU_TIMEOUT_CNT 300000
/* 200ms, uint:us*/
#define USB_ENUM_RESET_TIMEOUT 200000

volatile bool usb_initial = false;

static TaskHandle_t usb_task_handle = NULL;
static QueueHandle_t usb_queue_handle = NULL;

extern void usb_hisr(void);
extern hal_usb_status_t hal_usb_drv_create_isr(void);

bool is_vusb_ready(void)
{
#ifdef MTK_USB11
    bool ret;
    hal_gpio_status_t status;
    hal_gpio_data_t input_gpio_data = HAL_GPIO_DATA_LOW;

    status = hal_gpio_get_input((hal_gpio_pin_t)HAL_GPIO_36, &input_gpio_data);

    if (status == HAL_GPIO_STATUS_OK) {
        if (input_gpio_data == HAL_GPIO_DATA_HIGH) {
            ret = true;
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
#else
    bool ret = TRUE;
#if (PRODUCT_VERSION != 3335)
    ret = (bool)pmu_get_usb_input_status();
#endif
    return ret;
#endif
}

void USB_Send_Message(usb_msg_type_t msg, void *data)
{
    usb_msg_t msgs;
    BaseType_t xHigherPriorityTaskWoken;
    BaseType_t ret;

    if (usb_queue_handle == NULL) {
        LOG_MSGID_I(USB_MAIN, "usb task not initlize", 0);
        return;
    }

    // We have not woken a task at the start of the ISR.
    xHigherPriorityTaskWoken = pdFALSE;

    msgs.msg_id = msg;
    msgs.data = data;

    if (0 == HAL_NVIC_QUERY_EXCEPTION_NUMBER) {
        ret = xQueueSend(usb_queue_handle, &msgs, 0);
        LOG_MSGID_I(USB_MAIN, "Send Queue in Task !! id = %d ", 1, msgs.msg_id);
    } else {
        ret = xQueueSendFromISR(usb_queue_handle, &msgs, &xHigherPriorityTaskWoken);
        LOG_MSGID_I(USB_MAIN, "Send Queue in ISR !! id = %d ", 1, msgs.msg_id);
    }

    if (ret != pdTRUE) {
        LOG_MSGID_E(USB_MAIN, "Send Queue fail!! Queue size = %d ", 1, USB_QUEUE_LENGTH);
    }
    // Now the buffer is empty we can switch context if necessary.
    if (xHigherPriorityTaskWoken) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(pdTRUE);
    }
}


static void usb_task_main(void *pvParameters)
{
    usb_msg_t msgs;

    static uint32_t stack_max = 0;
    uint32_t stack_cur = 0;
#if defined(MTK_USB_AUDIO_V1_ENABLE)
    uint32_t temp;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
#endif

    while (1) {
        if (usb_queue_handle != NULL) {
            if (xQueueReceive(usb_queue_handle, &msgs, portMAX_DELAY)) {

                LOG_MSGID_I(USB_MAIN, "QueueReceive ID  = %d", 1, msgs.msg_id);

                switch (msgs.msg_id) {
                    case USB_ACM_MSG:
                        LOG_MSGID_I(USB_MAIN, "USB_ACM_MSG", 0);
                        break;
#ifdef  __USB_MASS_STORAGE_ENABLE__
                    case USB_MSC_RX_MSG:
                        USB_Ms_State_Main(MSG_ID_USB_MSDRV_REC_DONE_CONF, (usb_ms_rec_done_conf_struct *) msgs.data);
                        LOG_MSGID_I(USB_MAIN, "USB_MSC_RX_MSG: 0x%x", 1, msgs.data);
                        break;
                    case USB_MSC_TX_MSG:
                        USB_Ms_State_Main(MSG_ID_USB_MSDRV_TRX_DONE_CONF, NULL);
                        LOG_MSGID_I(USB_MAIN, "USB_MSC_TX_MSG", 0);
                        break;
                    case USB_MSC_CLR_STALL_MSG:
                        USB_Ms_State_Main(MSG_ID_USB_MSDRV_CLEAR_STALL_REQ, NULL);
                        LOG_MSGID_I(USB_MAIN, "USB_MSC_TX_MSG", 0);
                        break;
                    case USB_MSC_RESET_IND_MSG:
                        USB_Ms_State_Main(MSG_ID_USB_MSDRV_RESET_IND, NULL);
                        LOG_MSGID_I(USB_MAIN, "USB_MSC_TX_MSG", 0);
                        break;
#endif
#if defined(MTK_USB_AUDIO_V1_ENABLE)
                    case USB_AUDIO_SET_INTERFACE:
                        bInterfaceNumber=(uint8_t)(((uint32_t)(msgs.data)) >> 16);
                        bAlternateSetting=(uint8_t)(((uint32_t)(msgs.data)) & 0x0000FFFF);

                        if(bInterfaceNumber == g_UsbAudio[0].stream_interface_id){
                            // Notify current freq to APP
                            if(USB_Audio[0].setsamplingrate_cb) {
                                USB_Audio[0].setsamplingrate_cb(g_UsbAudio[0].stream_ep_out_id, USB_Audio_Get_CurSamplingRate(0));
                            }
                            // Notify enable/disable event to APP
                            if(USB_Audio[0].setinterface_cb) {
                                USB_Audio[0].setinterface_cb(bInterfaceNumber, bAlternateSetting);
                            }

                            if(bAlternateSetting == 0){
                                LOG_MSGID_I(USB_MAIN, "TYM_audio1 speaker disable", 0);
                                hal_usb_disable_rx_endpoint(g_UsbAudio[0].stream_ep_out_id);
                                hal_usb_clear_rx_endpoint_fifo(g_UsbAudio[0].stream_ep_out_id, HAL_USB_EP_TRANSFER_ISO, false);
                                /* Reset buffer related variables. */
                                g_UsbAudio[0].rx_dma_buffer_is_full = 0;
                                g_UsbAudio[0].rx_dma_buffer_read = 0;
                                g_UsbAudio[0].rx_dma_buffer_write = 0;

                                #ifdef USB_HOST_DETECT_PS
                                g_usb_audio_spk1_enable = FALSE;
                                g_usb_audio_spk1_status = FALSE;
                                #endif
                            }else if (bAlternateSetting == 1){
                                LOG_MSGID_I(USB_MAIN, "TYM_audio1 speaker enable", 0);
                                /* Reset buffer related variables. */
                                g_UsbAudio[0].rx_dma_buffer_is_full = 0;
                                g_UsbAudio[0].rx_dma_buffer_read = 0;
                                g_UsbAudio[0].rx_dma_buffer_write = 0;
                                hal_usb_enable_rx_endpoint(g_UsbAudio[0].stream_ep_out_id, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, false);

                               #ifdef USB_HOST_DETECT_PS
                               g_usb_audio_spk1_enable = TRUE;
                               g_usb_audio_spk1_status = FALSE;
                               #endif
                            }
                        }else if(bInterfaceNumber == g_UsbAudio[1].stream_interface_id) {
                            // Notify current freq to APP
                            if(USB_Audio[1].setsamplingrate_cb) {
                                USB_Audio[1].setsamplingrate_cb(g_UsbAudio[1].stream_ep_out_id, USB_Audio_Get_CurSamplingRate(1));
                            }
                            // Notify enable/disable event to APP
                            if(USB_Audio[1].setinterface_cb) {
                                USB_Audio[1].setinterface_cb(bInterfaceNumber, bAlternateSetting);
                            }

                            if(bAlternateSetting == 0){
                                LOG_MSGID_I(USB_MAIN, "TYM_audio2 speaker disable", 0);
                                hal_usb_disable_rx_endpoint(g_UsbAudio[1].stream_ep_out_id);
                                hal_usb_clear_rx_endpoint_fifo(g_UsbAudio[1].stream_ep_out_id, HAL_USB_EP_TRANSFER_ISO, false);
                                /* Reset buffer related variables. */
                                g_UsbAudio[1].rx_dma_buffer_is_full = 0;
                                g_UsbAudio[1].rx_dma_buffer_read = 0;
                                g_UsbAudio[1].rx_dma_buffer_write = 0;

                                #ifdef USB_HOST_DETECT_PS
                                g_usb_audio_spk2_enable = FALSE;
                                g_usb_audio_spk2_status = FALSE;
                                #endif
                            }else if (bAlternateSetting == 1){
                                LOG_MSGID_I(USB_MAIN, "TYM_audio2 speaker enable", 0);
                                /* Reset buffer related variables. */
                                g_UsbAudio[1].rx_dma_buffer_is_full = 0;
                                g_UsbAudio[1].rx_dma_buffer_read = 0;
                                g_UsbAudio[1].rx_dma_buffer_write = 0;
                                hal_usb_enable_rx_endpoint(g_UsbAudio[1].stream_ep_out_id, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, false);

                                #ifdef USB_HOST_DETECT_PS
                                g_usb_audio_spk2_enable = TRUE;
                                g_usb_audio_spk2_status = FALSE;
                                #endif
                            }
                        }
#ifdef MTK_USB_AUDIO_MICROPHONE
                        else if(bInterfaceNumber == g_UsbAudio[0].stream_microphone_interface_id){
                            // Notify current freq to APP
                            if(USB_Audio[0].setsamplingrate_cb_mic) {
                                USB_Audio[0].setsamplingrate_cb_mic(0x80 | g_UsbAudio[0].stream_ep_in_id, USB_Audio_Get_Mic_CurSamplingRate(0));
                            }
                            // Notify enable event to APP
                            if(USB_Audio[0].setinterface_cb_mic) {
                                USB_Audio[0].setinterface_cb_mic(bInterfaceNumber, bAlternateSetting);
                            }

                            /* Enable/disable endpoint for microphone */
                            if(bAlternateSetting == 0){
                                hal_usb_disable_tx_endpoint(g_UsbAudio[0].stream_ep_in_id);
                                LOG_MSGID_I(USB_MAIN, "TYM_audio microphone disable", 0);
                            }else if(bAlternateSetting == 1){
                                hal_usb_enable_tx_endpoint(g_UsbAudio[0].stream_ep_in_id, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, false);
                                hal_usb_clear_tx_endpoint_fifo(g_UsbAudio[0].stream_ep_in_id, HAL_USB_EP_TRANSFER_ISO, false);
                                LOG_MSGID_I(USB_MAIN, "TYM_audio microphone enable", 0);
                            }
                        }
#endif
                        break;
                    case USB_AUDIO_SET_SAMPLING_FREQ:
                        LOG_MSGID_I(USB_MAIN, "speaker sample ep[%d],rate[%d]", 2, ((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x00FFFFFF);
                        if(((((uint32_t)(msgs.data)) >> 24) == 0x01) && USB_Audio[0].setsamplingrate_cb){
                            USB_Audio[0].setsamplingrate_cb(((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x00FFFFFF);

                        }
                        if(((((uint32_t)(msgs.data)) >> 24) == 0x02) && USB_Audio[1].setsamplingrate_cb){
                            USB_Audio[1].setsamplingrate_cb(((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x00FFFFFF);
                        }
                        break;
#ifdef MTK_USB_AUDIO_MICROPHONE
                    case USB_MIC_SET_SAMPLING_FREQ:
                        LOG_MSGID_I(USB_MAIN, "mic sample ep[%d],rate[%d]", 2, ((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x00FFFFFF);
                        if(USB_Audio[0].setsamplingrate_cb_mic){
                            USB_Audio[0].setsamplingrate_cb_mic(((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x00FFFFFF);
                        }
                        break;
#endif
                    case USB_AUDIO_RX_DATA:
                        if(USB_Audio[0].rx_cb){
                            USB_Audio[0].rx_cb();
                        }
                        temp=g_UsbAudio[0].msg_notify;
                        if(temp>0){
                            temp--;
                            g_UsbAudio[0].msg_notify=temp;
                        }
                        break;
                    case USB_AUDIO_UNPLUG:
                        LOG_MSGID_I(USB_MAIN, "usb cable dissconnect[%d]", 0);
                        if(USB_Audio[0].unplug_cb){
                            USB_Audio[0].unplug_cb();
                        }
#ifdef MTK_USB_AUDIO_MICROPHONE
                        if(USB_Audio[0].unplug_cb_mic){
                            USB_Audio[0].unplug_cb_mic();
                        }
#endif
                        break;
                    case USB_AUDIO_SET_MUTE:
                        LOG_MSGID_I(USB_MAIN, "speaker mute ep[%d]mute[%d]", 2, ((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x000000FF);
                        if(((((uint32_t)(msgs.data)) >> 24) == 0x01) && USB_Audio[0].mute_cb){
                            USB_Audio[0].mute_cb(((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x000000FF);
                        } else if (((((uint32_t)(msgs.data)) >> 24) == 0x02) && USB_Audio[1].mute_cb) {
                            USB_Audio[1].mute_cb(((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x000000FF);
                        }
                        break;
#ifdef MTK_USB_AUDIO_MICROPHONE
                    case USB_MIC_SET_MUTE:
                        LOG_MSGID_I(USB_MAIN, "mic mute ep[%d]mute[%d]", 2, ((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x000000FF);
                        if(USB_Audio[0].mute_cb_mic){
                            USB_Audio[0].mute_cb_mic(((uint32_t)(msgs.data)) >> 24, ((uint32_t)(msgs.data)) & 0x000000FF);
                        }
                        break;
#endif
                    case USB_AUDIO_SET_VOLUME:
                        LOG_MSGID_I(USB_MAIN, "speaker sample ep[%d],ch[%d],vol[%d]", 3, ((uint32_t)(msgs.data)) >> 24, (((uint32_t)(msgs.data)) >> 16) & 0x000000FF, ((uint32_t)(msgs.data)) & 0x0000FFFF);
                        if(((((uint32_t)(msgs.data)) >> 24) == 0x01) && USB_Audio[0].volumechange_cb){
                            USB_Audio[0].volumechange_cb(((uint32_t)(msgs.data)) >> 24, (((uint32_t)(msgs.data)) >> 16) & 0x000000FF, ((uint32_t)(msgs.data)) & 0x0000FFFF);
                        } else if (((((uint32_t)(msgs.data)) >> 24) == 0x02) && USB_Audio[1].volumechange_cb) {
                            USB_Audio[1].volumechange_cb(((uint32_t)(msgs.data)) >> 24, (((uint32_t)(msgs.data)) >> 16) & 0x000000FF, ((uint32_t)(msgs.data)) & 0x0000FFFF);
                        }
                        break;
#ifdef MTK_USB_AUDIO_MICROPHONE
                    case USB_MIC_SET_VOLUME:
                        LOG_MSGID_I(USB_MAIN, "mic sample ep[%d],ch[%d],vol[%d]", 3, ((uint32_t)(msgs.data)) >> 24, (((uint32_t)(msgs.data)) >> 16) & 0x000000FF, ((uint32_t)(msgs.data)) & 0x0000FFFF);

                        if(USB_Audio[0].volumechange_cb_mic){
                            USB_Audio[0].volumechange_cb_mic(((uint32_t)(msgs.data)) >> 24, (((uint32_t)(msgs.data)) >> 16) & 0x000000FF, ((uint32_t)(msgs.data)) & 0x0000FFFF);
                        }
                        break;
#endif
#endif
                    case USB_ATCI_NOTIFY_UNPLUG:
                        usb_case_atci_call(false);
                        break;
                    case USB_ATCI_NOTIFY_PLUG:
                        usb_case_atci_call(true);
                        break;
                    case USB_RACE_NOTIFY_PLUG:
                        usb_case_race_call(true);
                        break;
                    case USB_RACE_NOTIFY_UNPLUG:
                        usb_case_race_call(false);
                        break;
#ifdef MTK_USB_AUDIO_HID_ENABLE
                    case USB_HID_SET_REPORT:
                        USB_Audio_HID_Ep0_DispatchData(NULL);
                        break;
                    case USB_HID_GET_REPORT:
                        break;
#endif
                    case USB_AP_REBOOT:
                        LOG_MSGID_I(USB_MAIN, "USB_AP_REBOOT AP_TYPE[%d]", 1, (int)msgs.data);
                        ap_usb_deinit();
                        ap_usb_init((USB_DEVICE_TYPE)msgs.data);
                        break;
#if defined(USB_HOST_DETECT_PS) && defined(MTK_USB_AUDIO_V1_ENABLE) && defined(MTK_USB_AUDIO_HID_ENABLE)
                    case USB_PS_DETECT:
                        LOG_MSGID_I(USB_MAIN, "USB_AP_REBOOT USB_PS_DETECT", 0);
                        USB_HostDetect_PlayStation();
                        break;
#endif
                    default:
                        break;
                }

                stack_cur = (USB_TASK_STACKSIZE - uxTaskGetStackHighWaterMark(usb_task_handle) * sizeof(portSTACK_TYPE));
                if (stack_cur > stack_max) {
                    stack_max = stack_cur;
                    LOG_MSGID_I(USB_MAIN, "task max-usage: %d", 1, stack_max);
                }

            }
        }
    }
}


bool usb_task_init(void)
{
    portBASE_TYPE xReturn = pdFALSE;

    /* Check task and queue is ready */
    if ((usb_queue_handle != NULL) || (usb_task_handle != NULL)) {
        LOG_MSGID_I(USB_MAIN, "usb_task_init task have inited, return !", 0);
        return true;
    }

    /* Queue creation */
    usb_queue_handle = xQueueCreate(USB_QUEUE_LENGTH, sizeof(usb_msg_t));

    if (usb_queue_handle == NULL) {
        LOG_MSGID_E(USB_MAIN, "usb_task_init queue create Fail!", 0);
        return false;
    }

    /* Task creation */
    xReturn = xTaskCreate(usb_task_main, USB_TASK_NAME, USB_TASK_STACKSIZE / sizeof(portSTACK_TYPE), NULL, USB_TASK_PRIO, &usb_task_handle);

    if (xReturn == pdFALSE) {
        vQueueDelete(usb_queue_handle);
        usb_queue_handle = NULL;
        LOG_MSGID_E(USB_MAIN, "usb_task_init task create Fail!", 0);
        return false;
    }

    LOG_MSGID_I(USB_MAIN, "usb_task_init success to create task and queue", 0);
    return true;
}

void usb_task_deinit(void)
{
    //for atci to notify task.
    #if defined(USB_ATCI_FEATURE)

    #else
        if (usb_queue_handle != NULL) {
            vQueueDelete(usb_queue_handle);
            usb_queue_handle = NULL;
        }

        if (usb_task_handle != NULL) {
            vTaskDelete(usb_task_handle);
            usb_task_handle = NULL;
        }
    #endif
}

void usb_dev_app_init_cdc_acm()
{
#ifdef USB_CDC_ACM_ENABLE
            USB_Register_CreateFunc("ACM COMMU1",
                                    USB_Acm1_CommIf_Create,
                                    USB_Acm1_CommIf_Reset,
                                    USB_Acm1_CommIf_Enable,
                                    (usb_speed_if_func_ptr)USB_Acm1_CommIf_Speed_Reset,
                                    NULL);

            USB_Register_CreateFunc("ACM DATA1",
                                    USB_Acm1_DataIf_Create,
                                    USB_Acm1_DataIf_Reset,
                                    USB_Acm1_DataIf_Enable,
                                    (usb_speed_if_func_ptr)USB_Acm1_DataIf_Speed_Reset,
                                    NULL);

            USB_Register_CreateFunc("ACM COMMU2",
                                    USB_Acm2_CommIf_Create,
                                    USB_Acm2_CommIf_Reset,
                                    USB_Acm2_CommIf_Enable,
                                    (usb_speed_if_func_ptr)USB_Acm2_CommIf_Speed_Reset,
                                    NULL);

            USB_Register_CreateFunc("ACM DATA2",
                                    USB_Acm2_DataIf_Create,
                                    USB_Acm2_DataIf_Reset,
                                    USB_Acm2_DataIf_Enable,
                                    (usb_speed_if_func_ptr)USB_Acm2_DataIf_Speed_Reset,
                                    NULL);

            USB_Init(USB_CDC_ACM);
#endif  
}

void usb_dev_app_init_msc()
{
#ifdef  __USB_MASS_STORAGE_ENABLE__
            g_UsbMS.ms_param = USB_GetMsParam();

            //USB_Ms_Register_DiskDriver(&USB_RAM_drv);
            USB_Ms_Register_DiskDriver(&usbms_msdc_driver);

#ifdef MTK_FATFS_ON_SERIAL_NAND
            USB_Ms_Register_DiskDriver(&usbms_spinand_driver);
#endif

            USB_Register_CreateFunc("MASS STORAGE", USB_Ms_If_Create, USB_Ms_If_Reset,
                                    USB_Ms_If_Enable, (usb_speed_if_func_ptr)USB_Ms_If_Speed_Reset, USB_Ms_If_Resume);

            /* initialize MASS STORAGE MODE */
            USB_Init(USB_MASS_STORAGE);
#endif
}

void usb_dev_app_init_hid()
{
#ifdef MTK_USB_HID_ENABLE
            USB_Register_CreateFunc("HID", USB_Hid_CommIf_Create, USB_Hid_CommIf_Reset,
                                    USB_Hid_CommIf_Enable, (usb_speed_if_func_ptr)USB_Hid_CommIf_Speed_Reset, NULL);

            USB_Init(USB_HID);
#endif
}

void usb_dev_app_init_audio()
{
#if defined(MTK_USB_AUDIO_V1_ENABLE)
    USB_Audio_Setting_By_NVKey();

    USB_Register_CreateFunc("AUDIO1 Control", USB_Audio1_ControlIf_Create, USB_Audio1_ControlIf_Reset,
                            USB_Audio1_ControlIf_Enable, (usb_speed_if_func_ptr)USB_Audio1_ControlIf_Speed_Reset, NULL);
    USB_Register_CreateFunc("AUDIO1 Stream", USB_Audio1_StreamIf_Create, USB_Audio1_StreamIf_Reset,
                            USB_Audio1_StreamIf_Enable, (usb_speed_if_func_ptr)USB_Audio1_StreamIf_Speed_Reset, NULL);
#ifdef MTK_USB_AUDIO_MICROPHONE
    USB_Register_CreateFunc("AUDIO Microphone", USB_Audio_StreamIf_Microphone_Create, USB_Audio_StreamIf_Microphone_Reset,
                            USB_Audio_StreamIf_Microphone_Enable, (usb_speed_if_func_ptr)USB_Audio_StreamIf_Microphone_Speed_Reset, NULL);
#endif
#ifdef MTK_USB_AUDIO_2ND_SPEAKER
    if(g_usb_audio_spk2_feature){
        USB_Register_CreateFunc("AUDIO2 Control", USB_Audio2_ControlIf_Create, USB_Audio2_ControlIf_Reset,
                                USB_Audio2_ControlIf_Enable, (usb_speed_if_func_ptr)USB_Audio2_ControlIf_Speed_Reset, NULL);
        USB_Register_CreateFunc("AUDIO2 Stream", USB_Audio2_StreamIf_Create, USB_Audio2_StreamIf_Reset,
                                USB_Audio2_StreamIf_Enable, (usb_speed_if_func_ptr)USB_Audio2_StreamIf_Speed_Reset, NULL);
    }
#endif
#endif

#ifdef MTK_USB_AUDIO_HID_ENABLE
    USB_Register_CreateFunc("AUDIO HID", USB_Audio_HID_Create, USB_Audio_HID_Reset,
                            USB_Audio_HID_Enable, (usb_speed_if_func_ptr)USB_Audio_HID_Speed_Reset, NULL);
#endif

    USB_Init(USB_AUDIO);
}

/*******************************************************
*   XBOX APP Initialise
*******************************************************/
#ifdef MTK_USB_XBOX_ENABLE
void usb_dev_app_init_xbox()
{
    LOG_MSGID_I(USB_MAIN, "usb_dev_app_init_xbox", 0);

    /* Create interface */
    USB_Register_CreateFunc("XBOX Control",
                             USB_XBOX_ControlIf_Create,
                             USB_XBOX_ControlIf_Reset,
                             USB_XBOX_ControlIf_Enable,
                             (usb_speed_if_func_ptr)USB_XBOX_ControlIf_Speed_Reset,
                             NULL);

    USB_Register_CreateFunc("XBOX Audio",
                             USB_XBOX_AudioIf_Create,
                             USB_XBOX_AudioIf_Reset,
                             USB_XBOX_AudioIf_Enable,
                             (usb_speed_if_func_ptr)USB_XBOX_AudioIf_Speed_Reset,
                             NULL);

    /* Initilize USB_XBOX */
    USB_Init(USB_XBOX);
}
#endif


/**
 * Senior: Dongle/Headset connect on Huawei SmartPhone.
 *
 * Huawei SmartPhone's phone call need below requirements.
 * 1. Device is in full-speed.
 * 2. Microphone supports 48k Hz.
 * 3. Only one USB audio device.
 */
void usb_dev_app_init_audio_smartphone()
{
#if defined(MTK_USB_AUDIO_V1_ENABLE)
    USB_Audio_Setting_By_NVKey();

    USB_Register_CreateFunc("AUDIO1 Control", USB_Audio1_ControlIf_Create, USB_Audio1_ControlIf_Reset,
                            USB_Audio1_ControlIf_Enable, (usb_speed_if_func_ptr)USB_Audio1_ControlIf_Speed_Reset, NULL);
    USB_Register_CreateFunc("AUDIO1 Stream", USB_Audio1_StreamIf_Create, USB_Audio1_StreamIf_Reset,
                            USB_Audio1_StreamIf_Enable, (usb_speed_if_func_ptr)USB_Audio1_StreamIf_Speed_Reset, NULL);
#ifdef MTK_USB_AUDIO_MICROPHONE
    USB_Register_CreateFunc("AUDIO Microphone", USB_Audio_StreamIf_Microphone_Create, USB_Audio_StreamIf_Microphone_Reset,
                            USB_Audio_StreamIf_Microphone_Enable, (usb_speed_if_func_ptr)USB_Audio_StreamIf_Microphone_Speed_Reset, NULL);
#endif
#endif

#ifdef MTK_USB_AUDIO_HID_ENABLE
    USB_Register_CreateFunc("AUDIO HID", USB_Audio_HID_Create, USB_Audio_HID_Reset,
                            USB_Audio_HID_Enable, (usb_speed_if_func_ptr)USB_Audio_HID_Speed_Reset, NULL);
#endif

    USB_Init(USB_AUDIO_SMARTPHONE);
}

/*  Refer to usb_dev_app_init_audio(), only enable HID for logging.
    Senior: headset connect with dongle by BT which can't accept USB-Audio in headset side;
             otherwise, headset disconnect with dongle.
    Becase this scenior, headset side can't enable USB-Audio(only enalbe USB-HID) for logging.
*/
void usb_dev_app_init_hid_logging()
{
#ifdef MTK_USB_AUDIO_HID_ENABLE
    USB_Register_CreateFunc("AUDIO HID", USB_Audio_HID_Create, USB_Audio_HID_Reset,
                            USB_Audio_HID_Enable, (usb_speed_if_func_ptr)USB_Audio_HID_Speed_Reset, NULL);
#endif

#if (defined(MTK_USB_AUDIO_HID_ENABLE))
    USB_Init(USB_AUDIO);
#endif
}


#if 0
usb_init_status_t usb_common_get_sleep()
{
    usb_init_status_t status = usb_common_init_fail;

    if (usb_sleep_handler == 0)
    {
        usb_sleep_handler = hal_sleep_manager_set_sleep_handle("USBDemo");
        status = usb_common_init_sucess;
    }
    
    return status;
}
#endif

usb_init_status_t usb_common_get_bc12(uint8_t *bc12_type)
{
    usb_init_status_t ret = usb_common_init_bc12_fail;

    #ifdef MTK_BATTERY_MANAGEMENT_ENABLE
        #if (PRODUCT_VERSION == 2822)
            *bc12_type = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_TYPE);
        #elif (PRODUCT_VERSION == 1565) //No BC12 hardware
            *bc12_type = SDP_CHARGER;
        #endif

        ret = usb_common_init_sucess;
    #endif

    LOG_MSGID_I(USB_MAIN, "usb_common_get_bc12 ret[%d] type[%d]", 2, ret, *bc12_type);

    return ret;
}

bool ap_usb_init(USB_DEVICE_TYPE type)
{
    uint32_t mask_irq = 0;

    hal_nvic_save_and_set_interrupt_mask(&mask_irq);
    if (usb_initial == true) {
        hal_nvic_restore_interrupt_mask(mask_irq);
        return false;
    }

    usb_initial = true;
    hal_nvic_restore_interrupt_mask(mask_irq);

    /*LOG_I(USB_MAIN,"++++++++++ usb lcok sleep\n");*/
#ifndef HAL_SLEEP_MANAGER_SUPPROT_DEEPSLEEP_LOCK
    hal_sleep_manager_lock_sleep(SLEEP_LOCK_USB);
#else
    hal_sleep_manager_acquire_sleeplock(usb_sleep_handler, HAL_SLEEP_LOCK_ALL);
#endif
    hal_usb_init();

    LOG_MSGID_I(USB_MAIN, "usb app init type [%d]", 1, type);

    switch (type) {
        case USB_CDC_ACM:
            usb_dev_app_init_cdc_acm();
            break;
        case USB_MASS_STORAGE:
            usb_dev_app_init_msc();
            break;
        case USB_HID:
            usb_dev_app_init_hid();
            break;
        case USB_AUDIO:
            usb_dev_app_init_audio();
            break;
        case USB_AUDIO_SMARTPHONE:
            usb_dev_app_init_audio_smartphone();
            break;
        case USB_HID_LOGGING:
            usb_dev_app_init_hid_logging();
            break;
#ifdef MTK_USB_XBOX_ENABLE
        case USB_XBOX:
            usb_dev_app_init_xbox();
            break;
#endif
        default:
            LOG_MSGID_E(USB_MAIN, "ap_usb_init : Fail", 0);
            break;
    }
    return true;
}

bool ap_usb_deinit(void)
{
    uint32_t mask_irq = 0;

    hal_nvic_save_and_set_interrupt_mask(&mask_irq);
    if (usb_initial == false) {
        hal_nvic_restore_interrupt_mask(mask_irq);
        return false;
    }
    hal_nvic_restore_interrupt_mask(mask_irq);

    /* Disable USB IRQ, otherwise USB irq would be tirggered during USB plug in/out (usb deinit stage) */
    hal_nvic_disable_irq(USB_IRQn);

    USB_Release_Type();

#ifndef HAL_SLEEP_MANAGER_SUPPROT_DEEPSLEEP_LOCK
    hal_sleep_manager_unlock_sleep(SLEEP_LOCK_USB);
#else
    //hal_sleep_manager_release_sleeplock(usb_sleep_handler, HAL_SLEEP_LOCK_ALL);
#endif
    /*LOG_I(USB_MAIN,"--------- usb unlcok sleep\n");*/
    hal_nvic_save_and_set_interrupt_mask(&mask_irq);
    usb_initial = false;
    hal_nvic_restore_interrupt_mask(mask_irq);

    return true;
}

void usb_cable_init_mt2822(void)
{
    LOG_MSGID_I(USB_MAIN, "usb_cable_init_mt2822", 0);

#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)

    /* Decide to enter logging mode from NVkey for headset project*/
    nvkey_status_t nvkey_state = NVKEY_STATUS_OK;
    uint32_t logging_mode_size = sizeof(uint8_t);
    uint8_t logging_mode_enable = 0;

    nvkey_state = nvkey_read_data(NVKEYID_APP_USB_LOGGING_MODE, &logging_mode_enable, &logging_mode_size);

    if(nvkey_state == NVKEY_STATUS_OK) {
        if (logging_mode_enable == true){
            LOG_MSGID_I(USB_MAIN,"Set logging model: enable[%x]", 1, logging_mode_enable);
            USB_Set_LoggingMode(true);
        } else {
            LOG_MSGID_I(USB_MAIN,"Set normal model: enable[%x]", 1, logging_mode_enable);
            USB_Set_LoggingMode(false);
        }
    } else {
        LOG_MSGID_I(USB_MAIN,"Set normal model by default nvkey_state[%x]", 1, nvkey_state);
        USB_Set_LoggingMode(false);
    }

    usb_init_status_t ret = usb_common_init_sucess;

    int i = 0;
    uint8_t charger_type = 0;

    if (is_vusb_ready()) 
    {
        if (usb_initial == true) {
            return; 
        } else {
            hal_usb_phy_preinit();
            ret &= usb_common_get_bc12(&charger_type);
            LOG_MSGID_I(USB_MAIN, "charger_type [%d]", 1, charger_type);
            USB_HostDetect_ResetRecorder();
        }
#if defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_XBOX_ENABLE)   
        usb_task_init();
#endif
#ifdef USB_DETECT_LED
        usb_detect_led(LED_GPIO, true);
#endif
#if defined(USB_ATCI_FEATURE)
        usb_task_init();
#endif

#if defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_HID_ENABLE) || defined(MTK_USB_XBOX_ENABLE)
        if(Get_USB_Host_Type() == USB_HOST_TYPE_XBOX){
            ap_usb_init(USB_XBOX);
        } else {
            if(USB_Get_LoggingMode() != true){
                ap_usb_init(USB_AUDIO);
            } else {
                ap_usb_init(USB_HID_LOGGING);
            }
        }
#else
        ap_usb_init(USB_CDC_ACM);
#endif

#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
        if ((charger_type == SDP_CHARGER) && (HAL_CORE_ACTIVE != hal_core_status_read(HAL_CORE_CM4)))
        {
            hal_nvic_disable_irq(USB_IRQn);
            do {
                usb_hisr();
                if (i++ >= BOOT_TIMEOUT_CNT) {
                    break;
                }
            } while ((USB_Get_Device_State() != DEVSTATE_CONFIG));

            LOG_MSGID_I(USB_MAIN, "USBCOM init, hisr cnt %d", 1, i);
            i = 0;
            /*call usb_hisr to handle EP0 callback, for usb logging before enable interrupt*/
            do {
                usb_hisr();
                if (i++ >= 10000) {
                    break;
                }
            } while (1);
            hal_nvic_enable_irq(USB_IRQn);
            LOG_MSGID_I(USB_MAIN, "USBCOM init 2, hisr cnt %d", 1, i);
            USB_Send_Message(USB_ATCI_NOTIFY_PLUG, NULL);
            USB_Send_Message(USB_RACE_NOTIFY_PLUG, NULL);
        }else if ((SDP_CHARGER != charger_type) && (CDP_CHARGER != charger_type)) {
            LOG_MSGID_I(USB_MAIN, "usb not connect to PC, deinit usb", 1, i);
            USB_Send_Message(USB_ATCI_NOTIFY_UNPLUG, NULL);
            USB_Send_Message(USB_RACE_NOTIFY_UNPLUG, NULL);
            ap_usb_deinit();
            usb_task_deinit();
        }
#else
    /* Detect USB by itslef when battery mangement was disable for dongle project
       Use GPT timer to check reset signal from host until 200ms timeout
    */
    uint32_t enum_timer_start, enum_timer_end, enum_timer_duration;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &enum_timer_start);

    /* Default gUsbDevice.isReset is false until reset signal is coming */
    gUsbDevice.isReset = false;

    hal_nvic_disable_irq(USB_IRQn);
    do {
        usb_hisr();

        /* USB reset signal timeout 200ms */
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &enum_timer_end);
        hal_gpt_get_duration_count(enum_timer_start, enum_timer_end, &enum_timer_duration);
        if(enum_timer_duration > USB_ENUM_RESET_TIMEOUT){
            if(gUsbDevice.isReset == false){
                LOG_MSGID_E(USB_MAIN, "USBCOM init, detect reset fail !", 0);
                break;
            }
        }

        /* USB enumeration until configuration state */
        if (i++ >= BOOT_TIMEOUT_CNT) {
            break;
        }
    } while ((USB_Get_Device_State() != DEVSTATE_CONFIG));
    hal_nvic_enable_irq(USB_IRQn);

    LOG_MSGID_I(USB_MAIN, "USBCOM init, hisr cnt[%d] timer[%d]", 2, i, enum_timer_duration);

    USB_Send_Message(USB_ATCI_NOTIFY_PLUG, NULL);
    USB_Send_Message(USB_RACE_NOTIFY_PLUG, NULL);
#endif

#ifdef MTK_PORT_SERVICE_ENABLE
        if (g_serial_port_usb_callback[0] != NULL) {
            g_serial_port_usb_callback[0](SERIAL_PORT_DEV_USB_COM1, SERIAL_PORT_EVENT_USB_CONNECTION, NULL);
            LOG_MSGID_I(USB_MAIN, "USB1 CONNECTION callback", 0);
        }
        if (g_serial_port_usb_callback[1] != NULL) {
            g_serial_port_usb_callback[1](SERIAL_PORT_DEV_USB_COM2, SERIAL_PORT_EVENT_USB_CONNECTION, NULL);
            LOG_MSGID_I(USB_MAIN, "USB2 CONNECTION callback", 0);
        }
#endif
    } else {
        if (usb_initial == true) {
#ifdef MTK_PORT_SERVICE_ENABLE
            if (g_serial_port_usb_callback[0] != NULL) {
                g_serial_port_usb_callback[0](SERIAL_PORT_DEV_USB_COM1, SERIAL_PORT_EVENT_USB_DISCONNECTION, NULL);
                LOG_MSGID_I(USB_MAIN, "USB1 CONNECTION callback", 0);
            }
            if (g_serial_port_usb_callback[1] != NULL) {
                g_serial_port_usb_callback[1](SERIAL_PORT_DEV_USB_COM2, SERIAL_PORT_EVENT_USB_DISCONNECTION, NULL);
                LOG_MSGID_I(USB_MAIN, "USB2 CONNECTION callback", 0);
            }

#endif
            LOG_MSGID_I(USB_MAIN, "USB out", 0);
            USB_Send_Message(USB_ATCI_NOTIFY_UNPLUG, NULL);
            ap_usb_deinit();
            usb_task_deinit();
            hal_usb_save_current();
#ifdef USB_DETECT_LED
            usb_detect_led(LED_GPIO, false);
#endif

        } else {
            LOG_MSGID_I(USB_MAIN, "USB not plug in !", 0);
        }
    }
#endif
}

void usb_cable_detect(void)
{
    LOG_MSGID_I(USB_MAIN, "usb_cable_detect", 0);

    /* USB cable detect */
#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
    usb_cable_init_mt2822();
#else
#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
    int i = 0;
    uint8_t charger_type = 0;
#endif
#elif defined(AG3335)
    int i = 0;
#endif
    uint32_t mask_irq = 0;

    hal_nvic_save_and_set_interrupt_mask(&mask_irq);
    //if (usb_sleep_handler == 0) {
    //    usb_sleep_handler = hal_sleep_manager_set_sleep_handle("USBDemo");
    //}
    hal_nvic_restore_interrupt_mask(mask_irq);

#if defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_XBOX_ENABLE)
    usb_task_init();
#endif

    if (is_vusb_ready()) {
        if (usb_initial == true) {
            return; /**/
        } else {
#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
            /*get charger type first, otherwise can not get correct type after DP/DM switched to USB.*/
            hal_usb_phy_preinit();
            charger_type = battery_management_get_charger_type_bc12();
#endif
#endif
            USB_HostDetect_ResetRecorder();
        }
        LOG_MSGID_I(USB_MAIN, "USB in", 0);

#ifdef USB_DETECT_LED
        usb_detect_led(LED_GPIO, true);
#endif
#if defined(MTK_USB_BOOT_MSC)
#ifdef AM255X
        usb_task_init();
#endif
        ap_usb_init(USB_MASS_STORAGE);
#elif defined(MTK_USB_HID_ENABLE)
        ap_usb_init(USB_HID);
#elif defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_HID_ENABLE) || defined(MTK_USB_XBOX_ENABLE)
        if(Get_USB_Host_Type() == USB_HOST_TYPE_XBOX){
            ap_usb_init(USB_XBOX);
        } else {
            if(USB_Get_LoggingMode() != true){
            ap_usb_init(USB_AUDIO);
            } else {
                ap_usb_init(USB_HID_LOGGING);
            }
        }
#else //defined(MTK_USB_BOOT_ACM)
        ap_usb_init(USB_CDC_ACM);
#endif

#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
        /*This section of the code only excuted when usb connect to PC not adapter, and only excuted in system init stage.*/
        /*otherwise usb hot-plug maybe cause music to stop.*/
        if (((SDP_CHARGER == charger_type) || (CDP_CHARGER == charger_type)) && (HAL_CORE_ACTIVE != hal_core_status_read(HAL_CORE_CM4))) {
            hal_nvic_disable_irq(USB_IRQn);
            do {
                usb_hisr();
                if (i++ >= BOOT_TIMEOUT_CNT) {
                    break;
                }
            } while ((USB_Get_Device_State() != DEVSTATE_CONFIG));
            LOG_MSGID_I(USB_MAIN, "USBCOM init, hisr cnt%d", 1, i);

            i = 0;
            /*call usb_hisr to handle EP0 callback, for usb logging before enable interrupt*/
            do {
                usb_hisr();
                if (i++ >= 10000) {
                    break;
                }
            } while (1);
            hal_nvic_enable_irq(USB_IRQn);
            LOG_MSGID_I(USB_MAIN, "USBCOM init 2, hisr cnt%d", 1, i);
        } else if ((SDP_CHARGER != charger_type) && (CDP_CHARGER != charger_type)) {
            LOG_MSGID_I(USB_MAIN, "usb not connect to PC, deinit usb", 1, i);
            ap_usb_deinit();
            usb_task_deinit();
        }
#endif
#elif defined(AG3335)
        LOG_MSGID_I(USB_MAIN, "usb 3335 USBCOM init s, hisr cnt %d", 1, i);
            hal_nvic_disable_irq(USB_IRQn);
        do {
            usb_hisr();
            i++;
        } while ((i < 900000));
        hal_nvic_enable_irq(USB_IRQn);
        USB_Set_Device_Status();
        LOG_MSGID_I(USB_MAIN, "usb 3335 USBCOM init e, hisr cnt %d", 1, i);
#endif
#ifdef MTK_PORT_SERVICE_ENABLE
        if (g_serial_port_usb_callback[0] != NULL) {
            g_serial_port_usb_callback[0](SERIAL_PORT_DEV_USB_COM1, SERIAL_PORT_EVENT_USB_CONNECTION, NULL);
            LOG_MSGID_I(USB_MAIN, "USB1 CONNECTION callback", 0);
        }
        if (g_serial_port_usb_callback[1] != NULL) {
            g_serial_port_usb_callback[1](SERIAL_PORT_DEV_USB_COM2, SERIAL_PORT_EVENT_USB_CONNECTION, NULL);
            LOG_MSGID_I(USB_MAIN, "USB2 CONNECTION callback", 0);
        }
#endif

#ifdef  __USB_MASS_STORAGE_ENABLE__
        if (g_msc_usb_callback != NULL) {
            g_msc_usb_callback(MSC_EVENT_USB_CONNECTION);
            LOG_MSGID_I(USB_MAIN, "msc CONNECTION callback", 0);
        }
#endif


    } else {
        if (usb_initial == true) {
#ifdef MTK_PORT_SERVICE_ENABLE
            if (g_serial_port_usb_callback[0] != NULL) {
                g_serial_port_usb_callback[0](SERIAL_PORT_DEV_USB_COM1, SERIAL_PORT_EVENT_USB_DISCONNECTION, NULL);
                LOG_MSGID_I(USB_MAIN, "USB1 CONNECTION callback", 0);
            }
            if (g_serial_port_usb_callback[1] != NULL) {
                g_serial_port_usb_callback[1](SERIAL_PORT_DEV_USB_COM2, SERIAL_PORT_EVENT_USB_DISCONNECTION, NULL);
                LOG_MSGID_I(USB_MAIN, "USB2 CONNECTION callback", 0);
            }

#endif

#ifdef  __USB_MASS_STORAGE_ENABLE__
            if (g_msc_usb_callback != NULL) {
                g_msc_usb_callback(MSC_EVENT_USB_DISCONNECTION);
                LOG_MSGID_I(USB_MAIN, "msc DISCONNECTION callback", 0);
            }
#endif
            LOG_MSGID_I(USB_MAIN, "USB out", 0);
            ap_usb_deinit();
#if defined(MTK_USB_AUDIO_V1_ENABLE)
            // We cannot delete USB task and related queue, since we need to pass unplug message to
            // it and will call the user provided unplug callback in USB task.
#else
            usb_task_deinit();
#endif

#ifdef USB_DETECT_LED
            usb_detect_led(LED_GPIO, false);
#endif
        } else {
            LOG_MSGID_I(USB_MAIN, "USB not plug in !", 0);
#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
            hal_usb_save_current();
#endif
        }
    }

#endif

    usb_atci_init();
}

bool usb_device_test_case(usb_device_case_type_t usb_case)
{
    bool ret_value = false;

    LOG_MSGID_I(USB_MAIN, "usb_device_test_case case [%d]", 1, (int)usb_case);

    uint32_t i = 0;

    switch(usb_case)
    {
        case usb_device_slt:
            ap_usb_init(USB_CDC_ACM);

            do {
                usb_hisr();
                if (i++ >= BOOT_TIMEOUT_CNT) {
                    break;
                }
            } while ((USB_Get_Device_State() != DEVSTATE_CONFIG));

            if(USB_Get_Device_State() == DEVSTATE_CONFIG){
                ret_value = true;
                LOG_MSGID_I(USB_MAIN, "usb_device_test_case slt pass", 0);
            }else{
                ret_value = false;
                LOG_MSGID_I(USB_MAIN, "usb_device_test_case slt fail", 0);
            }
            ap_usb_deinit();
            break;
        default:
            break;
    }

    return ret_value;
}



void usb_boot_init(void)
{
    int i = 0;

    usb_task_init();
    hal_usb_drv_create_isr();

    //if (usb_sleep_handler == 0) {
    //    usb_sleep_handler = hal_sleep_manager_set_sleep_handle("USBDemo");
    //    LOG_MSGID_I(USB_MAIN, "usb_sleep_handler=%d\n", 1, usb_sleep_handler);
    //}

    if (is_vusb_ready()) {
        LOG_MSGID_I(USB_MAIN, "Boot-USB_in", 0);

#if defined(MTK_USB_BOOT_MSC)
        ap_usb_init(USB_MASS_STORAGE);
#else //defined(MTK_USB_BOOT_MSC)
        ap_usb_init(USB_CDC_ACM);
        LOG_MSGID_I(USB_MAIN, "ap_usb_init(USB_CDC_ACM)", 0);
#endif
        do {
            usb_hisr();
            if (i++ >= BOOT_TIMEOUT_CNT) {
                break;
            }
        } while ((USB_Get_Device_State() != DEVSTATE_CONFIG));
        LOG_MSGID_I(USB_MAIN, "USBCOM init, hisr cnt %d", 1, i);
    } else {
        /*call usb phy preint for current save & BC1.2 control PD/PM*/
#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
        hal_usb_save_current();
#endif
        LOG_MSGID_I(USB_MAIN, "USB not plug in !\n", 0);
    }
}

bool usb_entry_force_enumeration()
{
    bool ret = true;
    uint32_t count = 0;

    do {
        usb_hisr();

        if (count++ >= BOOT_TIMEOUT_CNT) {
            ret = false;
            break;
        }
    } while ((USB_Get_Device_State() != DEVSTATE_CONFIG));

    LOG_MSGID_I(USB_MAIN, "usb usb_entry_force_enumeration step1 [%d]\r", 1, count);

    count= 0;

    do {
        usb_hisr();
        if (count++ >= 10000) {
            break;
        }
    } while (1);

    LOG_MSGID_I(USB_MAIN, "usb usb_entry_force_enumeration step2 [%d]\n\r", 1, count);

    return ret;
}

void usb_entry_case_boot()
{
#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
    bool status = false;
    hal_usb_status_t hal_status = HAL_USB_STATUS_OK;
    uint8_t charger_type = 0;

    status = is_vusb_ready();
    if(status)
    {

        status = usb_task_init();
        if(status == false)
            LOG_MSGID_I(USB_MAIN, "usb task init fail", 0);

        hal_usb_phy_preinit();
        #ifdef MTK_BATTERY_MANAGEMENT_ENABLE
            #if (PRODUCT_VERSION == 2822)
                charger_type = pmu_get_bc12_charger_type();
            #elif (PRODUCT_VERSION == 1565)
                charger_type = pmu_get_bc12_charger_type();
            #endif
        #endif
        LOG_MSGID_I(USB_MAIN, "usb charger_type [%d]", 1, charger_type);

        hal_status = hal_usb_drv_create_isr();
        if(hal_status != HAL_USB_STATUS_OK)
            LOG_MSGID_I(USB_MAIN, "usb isr init fail", 0);

        //if (usb_sleep_handler == 0) {
        //    usb_sleep_handler = hal_sleep_manager_set_sleep_handle("USBDemo");
        //    LOG_MSGID_I(USB_MAIN, "usb_sleep_handler=%d\n", 1, usb_sleep_handler);
        //}

        if (charger_type == 0) {
            ap_usb_deinit();
            usb_task_deinit();
            usb_initial = false;
        }
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
        else if (charger_type == SDP_CHARGER)
        {
            ap_usb_init(USB_CDC_ACM);
            usb_entry_force_enumeration();
            usb_initial = true;
        }
#endif
    }else{
        ap_usb_deinit();
    }
#endif
}



#ifdef  __USB_MASS_STORAGE_ENABLE__
void ap_usb_register_msc_callback(msc_register_callback_t callback)
{
    g_msc_usb_callback = callback;
    LOG_MSGID_I(USB_MAIN, "register MSC connection callback\n", 0);

    if (g_msc_usb_callback != NULL) {
        if (is_vusb_ready()) {
            g_msc_usb_callback(MSC_EVENT_USB_CONNECTION);
            LOG_MSGID_I(USB_MAIN, "MSC_CONNECTION callback\n", 0);
        } else {
            g_msc_usb_callback(MSC_EVENT_USB_DISCONNECTION);
            LOG_MSGID_I(USB_MAIN, "MSC_DISCONNECTION callback\n", 0);
        }
    }
}
#endif

/*
 * After the boot, could be through the led's gpio and vibr power domain to control led status
 * LED light: GPIO = low  and VIBR = on
 * LED dark : GPIO = high and VIBR = off
 * */
#ifdef USB_DETECT_LED
void usb_detect_led(hal_gpio_pin_t gpio_index, bool status)
{

    if (status == true) {
        LOG_MSGID_I(USB_MAIN, "LED Light", 0);
        hal_gpio_init(gpio_index);
        hal_pinmux_set_function(gpio_index, 0);/* Set pin as GPIO mode.*/
        hal_gpio_set_direction(gpio_index, HAL_GPIO_DIRECTION_OUTPUT); /* Set GPIO as output.*/
        hal_gpio_set_output(gpio_index, HAL_GPIO_DATA_LOW); /*set gpio output low*/

        PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VIBR, VIBR_VOL); /*set vibr voltage */
        PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_ENABLE); /*vibr enable*/
    } else {
        LOG_MSGID_I(USB_MAIN, "LED off", 0);
        hal_gpio_init(gpio_index);
        hal_pinmux_set_function(gpio_index, 0);
        hal_gpio_set_direction(gpio_index, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_output(gpio_index, HAL_GPIO_DATA_HIGH);

        PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VIBR, VIBR_VOL);
        PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_DISABLE);
    }
}

#endif

/* Set/Get USB Host Type */
USB_HOST_TYPE g_usb_host_type = USB_HOST_TYPE_UNKNOWN;

USB_HOST_TYPE Set_USB_Host_Type(USB_HOST_TYPE usb_host_type)
{
    LOG_MSGID_I(USB_MAIN, "Set_USB_Host_Type[%x]", 1, usb_host_type);
    g_usb_host_type = usb_host_type;
    return g_usb_host_type;
}

USB_HOST_TYPE Get_USB_Host_Type()
{
    LOG_MSGID_I(USB_MAIN, "Get_USB_Host_Type[%x]", 1, g_usb_host_type);
    return g_usb_host_type;
}

extern USB_HOST_TYPE Set_USB_Host_Type(USB_HOST_TYPE usb_host_type);
extern USB_HOST_TYPE Get_USB_Host_Type();


#endif //MTK_USB_DEMO_ENABLED


