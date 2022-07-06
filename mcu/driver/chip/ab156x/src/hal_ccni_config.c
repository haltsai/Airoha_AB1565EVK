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
    //log_hal_msgid_error("[CM4] default_isr event = 0x%x \r\n",1, event);
    hal_ccni_clear_event(event);
    hal_ccni_unmask_event(event);
}
#if defined(HAL_AUDIO_MODULE_ENABLED)
extern void hal_audio_ccni_isr(hal_ccni_event_t event, void * msg);
#else
#define hal_audio_ccni_isr default_isr
#endif /* HAL_AUDIO_MODULE_ENABLED */

#ifdef CCCI_ENABLE
extern void ccci_cm4_receive_msg_from_others_core(hal_ccni_event_t event, void * msg);
extern void ccci_cm4_receive_msg_from_others_core_for_ccci_ack(hal_ccni_event_t event, void * msg);
#else
#define ccci_cm4_receive_msg_from_others_core default_isr
#define ccci_cm4_receive_msg_from_others_core_for_ccci_ack default_isr
#endif /* CCCI_ENABLE */

#if (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
extern void bt_controller_receive_ull_encode_started_msg_from_dsp0(hal_ccni_event_t event, void * msg);
#else
#define bt_controller_receive_ull_encode_started_msg_from_dsp0 default_isr
#endif

volatile bool exception_triggered_flag = FALSE;
// It is ISR for triggered exception event.
void trigger_exception(hal_ccni_event_t event, void * msg)
{
    exception_triggered_flag = TRUE;
    ATTR_LOG_STRING exp[] = "Triggered";
    ATTR_LOG_STRING file[] = __FILE__;
    light_assert(exp, file, __LINE__);
    (void) event;
    (void) msg;
}

//please should replace dafult_callback with user callback if the event is used
const hal_ccni_function_t ccni_dsp0_to_cm4_function_table[CCNI_DSP0_TO_CM4_EVENT_MAX] =
{
    {hal_audio_ccni_isr}, /*CCNI_DSP0_TO_CM4_TX_EVENT*/
    {default_isr},        /*DSP0_TO_CM4_WITH_MSG_EVENT1*/
    {bt_controller_receive_ull_encode_started_msg_from_dsp0},        /*DSP0_TO_CM4_WITH_MSG_EVENT2*/
    {default_isr},        /*DSP0_TO_CM4_WITH_MSG_EVENT3*/
    {default_isr},        /*DSP0_TO_CM4_WITH_MSG_EVENT4*/
    {default_isr},        /*DSP0_TO_CM4_WITH_MSG_EVENT5*/
    {ccci_cm4_receive_msg_from_others_core_for_ccci_ack},  /*DSP0_TO_CM4_WITH_MSG_EVENT6*/
    {ccci_cm4_receive_msg_from_others_core},               /*DSP0_TO_CM4_WITH_MSG_EVENT7*/
    {trigger_exception},  /*DSP0_TO_CM4_WITHOUT_MSG_EVENT8*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT9*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT10*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT11*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT12*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT13*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT14*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT15*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT16*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT17*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT18*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT19*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT20*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT21*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT22*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT23*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT24*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT25*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT26*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT27*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT28*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT29*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT30*/
    {default_isr},        /*DSP0_TO_CM4_WITHOUT_MSG_EVENT31*/
}; //CM4


#ifdef __cplusplus
}
#endif

#endif /* HAL_CCNI_MODULE_ENABLED */

