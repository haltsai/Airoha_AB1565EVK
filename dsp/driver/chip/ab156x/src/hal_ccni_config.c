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
 
#include "hal_ccni.h"

#ifdef HAL_CCNI_MODULE_ENABLED
#include "hal_ccni_config.h"
#include "hal_log.h"
#include "exception_handler.h"
#include "memory_attribute.h"

#ifdef __cplusplus
extern "C" {
#endif

void default_isr(hal_ccni_event_t event, void * msg)
{
    (void) msg;
    hal_ccni_mask_event(event);
    //log_hal_msgid_error("[DSP0] default_isr event = 0x%x \r\n",1, event);
    hal_ccni_clear_event(event);
    hal_ccni_unmask_event(event);
}

#ifdef CCCI_ENABLE
extern void ccci_dsp0_receive_msg_from_others_core(hal_ccni_event_t event, void * msg);
extern void ccci_dsp0_receive_msg_from_others_core_for_ccci_ack(hal_ccni_event_t event, void * msg);
#else
#define ccci_dsp0_receive_msg_from_others_core  default_isr
#define ccci_dsp0_receive_msg_from_others_core_for_ccci_ack  default_isr
#endif /* CCCI_ENABLE */

#ifdef MTK_DSP_AUDIO_MESSAGE_ENABLE
extern void aud_msg_rx_handler(hal_ccni_event_t event, void *msg);
#else
#define aud_msg_rx_handler  default_isr
#endif /* MTK_DSP_AUDIO_MESSAGE_ENABLE */

#if MTK_AUDIO_TRANSMITTER_ENABLE
extern void audio_transmitter_source_ccni_handler(hal_ccni_event_t event, void * msg);
#else
#define audio_transmitter_source_ccni_handler default_isr
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */

#if MTK_AUDIO_BT_COMMON_ENABLE
extern void bt_common_source_ccni_handler(hal_ccni_event_t event, void * msg);
#else
#define bt_common_source_ccni_handler default_isr
#endif /* MTK_AUDIO_BT_COMMON_ENABLE */

// It is ISR for triggered exception event.
void trigger_exception(hal_ccni_event_t event, void * msg)
{
    ATTR_LOG_STRING exp[] = "Triggered";
    ATTR_LOG_STRING file[] = __FILE__;
    light_assert(exp, file, __LINE__);
    (void) event;
    (void) msg;
}

//please should replace dafult_callback with user callback if the event is used
const hal_ccni_function_t ccni_cm4_to_dsp0_function_table[CCNI_CM4_TO_DSP0_EVENT_MAX] =
{
    {aud_msg_rx_handler}, /*CCNI_CM4_TO_DSP0_RX_EVENT*/
    {default_isr},        /*CM4_TO_DSP0_WITH_MSG_EVENT1*/
    {audio_transmitter_source_ccni_handler},                  /*CM4_TO_DSP0_WITH_MSG_EVENT2*/
    {bt_common_source_ccni_handler},                          /*CM4_TO_DSP0_WITH_MSG_EVENT3*/
    {default_isr},        /*CM4_TO_DSP0_WITH_MSG_EVENT4*/
    {default_isr},        /*CM4_TO_DSP0_WITH_MSG_EVENT5*/
    {ccci_dsp0_receive_msg_from_others_core_for_ccci_ack},    /*CM4_TO_DSP0_WITH_MSG_EVENT6*/
    {ccci_dsp0_receive_msg_from_others_core},                 /*CM4_TO_DSP0_WITH_MSG_EVENT7*/
    {trigger_exception},  /*CM4_TO_DSP0_WITHOUT_MSG_EVENT8*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT9*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT10*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT11*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT12*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT13*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT14*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT15*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT16*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT17*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT18*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT19*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT20*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT21*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT22*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT23*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT24*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT25*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT26*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT27*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT28*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT29*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT30*/
    {default_isr},        /*CM4_TO_DSP0_WITHOUT_MSG_EVENT31*/
};


#ifdef __cplusplus
}
#endif

#endif /* HAL_CCNI_MODULE_ENABLED */

