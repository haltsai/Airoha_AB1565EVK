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
#include "hal_resource_assignment.h"
#include "hal_ccni_internal.h"
#include "memory_attribute.h"
#include "hal_nvic.h"
#include "hal_log.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define CCNI_DEBUG

#define IRQ_MASK_OFFSET       (0x0)
#define IRQ_SET_OFFSET        (0x4)
#define IRQ_CLR_OFFSET        (0x8)
#define IRQ_STATUS_OFFSET     (0xc)
#define CCNI_MAX_EVENT        (0x20)
#define CCNI_EVENT_MASK_EN    (0x1)
#define CCNI_EVENT_UNMASK_EN  (0x0)
#define CCNI_EVENT_CLEAR_EN   (0x1)

#define CCNI_EVENT_EN         (0x1)

#define CCNI_MAX_MSG          (0x8)

static void ccni_irq_handler(hal_nvic_irq_t irq);
extern void exception_handler(hal_ccni_event_t event, void * msg);

static void clear_all_events(void)
{
    CM4_CCNI->IRQ0_CLR = CCNI_REG_EVENTS_CLEAR;
    DSP0_CCNI->IRQ0_CLR = CCNI_REG_EVENTS_CLEAR;
}

hal_ccni_status_t hal_ccni_init(void)
{
    hal_ccni_status_t status;
    //clear all event when at CM4 side.
    clear_all_events();
    hal_nvic_register_isr_handler(DSP0_IRQn,ccni_irq_handler);
    hal_nvic_enable_irq(DSP0_IRQn);
    return (status = HAL_CCNI_STATUS_OK);
}

static void ccni_irq_handler(hal_nvic_irq_t irq)
{
    uint32_t i;
    uint32_t event;
    uint32_t msg_data[2];
    uint32_t *p_share_memory;

    (void)irq;
    for (i = 0; i < CCNI_MAX_EVENT; i++) {
        uint32_t irq_event;
        irq_event = DSP0_CCNI->IRQ0_STATUS;
        event = IRQGEN_DSP0_TO_CM4_EXCEPTION;

        if (irq_event & (0x1<< (event&CCNI_EVENT_MASK))) {
            //handle exception at first
            hal_ccni_mask_event(event);
            ccni_dsp0_to_cm4_function_table[event & CCNI_EVENT_MASK].hal_ccni_callback(event, NULL);
            //Exception can't reach here
        }

        if (irq_event & (1 << i)) {
            event = i | CCNI_EVENT_SRC_DSP0 | CCNI_EVENT_DST_CM4;
            if((DSP0_CCNI->IRQ0_MASK)&(CCNI_REG_ONE_BIT_STATUS<<i)){
                //do nothing if event is marked
                #ifdef CCNI_DEBUG
                log_hal_msgid_info("[CM4]:event =0x%x is masked !!!\r\n", 1, event);
                #endif
                continue;
            }

            hal_ccni_mask_event(event);
            if (ccni_dsp0_to_cm4_function_table[i].hal_ccni_callback) {
                if (i < CCNI_MAX_MSG) {
                    p_share_memory = (uint32_t*)(CCNI_MSG_BASE + CCNI_CM4_TO_DSP0_MSG_SIZE);
                    msg_data[0] = p_share_memory[2*(i)];
                    msg_data[1] = p_share_memory[2*(i)+1];
                    #ifdef CCNI_DEBUG
                    log_hal_msgid_info("[CM4 Receive]:event=0x%x, msg_0=0x%x, msg_1=0x%x \r\n",3,event,msg_data[0], msg_data[1]);
                    #endif
                    ccni_dsp0_to_cm4_function_table[i].hal_ccni_callback(event, msg_data);
                    #ifdef CCNI_DEBUG
                    log_hal_msgid_info("[CM4 Receive]: callback is exit \r\n", 0);
                    #endif
                } else {
                    #ifdef CCNI_DEBUG
                    log_hal_msgid_info("[CM4 Receive]:event=0x%x \r\n", 1, irq_event);
                    #endif
                    ccni_dsp0_to_cm4_function_table[i].hal_ccni_callback(event, NULL);
                }
            }
        }
    }
}

//Query event status
hal_ccni_status_t hal_ccni_query_event_status(hal_ccni_event_t event, uint32_t *data)
{
    hal_ccni_status_t status = HAL_CCNI_STATUS_OK;
    uint32_t mask;

    //Not support to query self to self event, and event is less than max defined event
    if (data == NULL || ((event&CCNI_SRC_MASK) == ((event&CCNI_DST_MASK)<<8)) || (((event&CCNI_EVENT_MASK)>=CCNI_MAX_EVENT))) {
         return (status = HAL_CCNI_STATUS_INVALID_PARAMETER);
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    if((event & CCNI_EVENT_DST_DSP0) != 0) {
        *data = ((CM4_CCNI->IRQ0_STATUS)&(CCNI_REG_ONE_BIT_STATUS<<(event&CCNI_EVENT_MASK))) ? HAL_CCNI_EVENT_STATUS_BUSY : HAL_CCNI_EVENT_STATUS_IDLE;
    } else if((event & CCNI_EVENT_DST_CM4) != 0) {
        *data = ((DSP0_CCNI->IRQ0_STATUS)&(CCNI_REG_ONE_BIT_STATUS<<(event&CCNI_EVENT_MASK))) ? HAL_CCNI_EVENT_STATUS_BUSY : HAL_CCNI_EVENT_STATUS_IDLE;
    }
    hal_nvic_restore_interrupt_mask(mask);
    return status;
}

/**
 * @brief This function trigger an an intterupt of the destination processer .
 * @param[in] dcore is the interrupt receiver.
 * @param[in] event is the intterrupt name
 * @param[in] message is optional
 */
hal_ccni_status_t hal_ccni_set_event(hal_ccni_event_t event, hal_ccni_message_t *message)
{
    hal_ccni_status_t status = HAL_CCNI_STATUS_OK;
    uint32_t mask;
    uint32_t data;
    uint32_t* p_share_memory;
    uint32_t event_index;

    //only supports CM4 to clear received event
    if ((CCNI_EVENT_SRC_CM4!=(event&CCNI_SRC_MASK)) || (CCNI_EVENT_DST_CM4==(event&CCNI_DST_MASK))
        || ((event&CCNI_EVENT_MASK)>=CCNI_MAX_EVENT)) {
        return (status = HAL_CCNI_STATUS_INVALID_PARAMETER);
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    //check event if it is busy
    status = hal_ccni_query_event_status(event, &data);
    if (status != HAL_CCNI_STATUS_OK) {
        hal_nvic_restore_interrupt_mask(mask);
        return status;
    }
    if (data == HAL_CCNI_EVENT_STATUS_BUSY) {
        hal_nvic_restore_interrupt_mask(mask);
        return (status = HAL_CCNI_STATUS_BUSY);
    }

    event_index = (event & CCNI_EVENT_MASK);
    if (event_index < CCNI_MAX_MSG) {
        p_share_memory = (uint32_t*)(CCNI_MSG_BASE);
        p_share_memory[event_index*2] =message->ccni_message[0];
        p_share_memory[event_index*2 +1]=message->ccni_message[1];
    }

    (CM4_CCNI->IRQ0_SET) |= (CCNI_REG_ONE_BIT_SET<<event_index);
    hal_nvic_restore_interrupt_mask(mask);
    return status;
}


static hal_ccni_status_t ccni_write_register(hal_ccni_event_t event, uint32_t register_offset, uint32_t value)
{
    uint32_t reg_addr,mask;
    
    //only supports CM4 to clear event / mask event / unmask event
    if ((CCNI_EVENT_SRC_CM4 == (event&CCNI_SRC_MASK)) || (CCNI_EVENT_DST_CM4 != (event&CCNI_DST_MASK)) 
        || ((event&CCNI_EVENT_MASK) >= CCNI_MAX_EVENT)) {
        return HAL_CCNI_STATUS_INVALID_PARAMETER;
    }

#ifdef CCNI_DEBUG
    log_hal_msgid_info("[ccni_write_register]:event=0x%x, reg=0x%x, value =0x%x \r\n",3,event,register_offset,value);
#endif

    hal_nvic_save_and_set_interrupt_mask(&mask);
    reg_addr = DSP0_CCNI_BASE + register_offset;   //DSP0 to CM4
    if (value == CCNI_EVENT_EN) {
        (*(volatile uint32_t*)reg_addr)|=(0x1<<(event&CCNI_EVENT_MASK));
    } else {
        (*(volatile uint32_t*)reg_addr) &= (~(0x1<<(event&CCNI_EVENT_MASK)));
    }
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_CCNI_STATUS_OK;
}

//CM4 Clear the event from other processer after handler it
hal_ccni_status_t hal_ccni_clear_event(hal_ccni_event_t event)
{
    return ccni_write_register(event,IRQ_CLR_OFFSET,CCNI_EVENT_CLEAR_EN);
}

//CM4 MASK the event from other processer after handler it
hal_ccni_status_t hal_ccni_mask_event(hal_ccni_event_t event)
{
    return ccni_write_register(event,IRQ_MASK_OFFSET,CCNI_EVENT_MASK_EN);
}

//CM4 UNMASK the event from other processer after handler it
hal_ccni_status_t hal_ccni_unmask_event(hal_ccni_event_t event)
{
    return ccni_write_register(event,IRQ_MASK_OFFSET,CCNI_EVENT_UNMASK_EN);
}

hal_ccni_status_t hal_ccni_deinit(void)
{
    return HAL_CCNI_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_CCNI_MODULE_ENABLED */

