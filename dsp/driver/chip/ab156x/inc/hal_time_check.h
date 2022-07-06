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

#ifndef __HAL_TIME_CHECK_H__
#define __HAL_TIME_CHECK_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_feature_config.h"
#include "stdint.h"
#include "hal_gpt.h"

#ifdef HAL_TIME_CHECK_ENABLED

#define HAL_TIME_CHECK_ISR_ENABLED            //ISR handler execution time check feature

#ifdef HAL_TIME_CHECK_ISR_ENABLED
#define TIME_CHECK_ISR_TIME   5000              //Set the ISR execution time 5000us limit
#define ENABLE_IRQ_HANDLER_CHECK_ASSERT         //enable assert when the ISR handler execution time over than 5000us
#endif

#define HAL_TIME_CHECK_DISABLE_IRQ_ENABLED       //IRQ masked time check feature

#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
#define TIME_CHECK_DISABLE_IRQ_TIME  5000        //Set the IRQ disable time 5000us limit
#define ENABLE_REAL_TIME_CHECK_ASSERT              //enable assert when the IRQ disable time over than 500us

#endif

#ifdef HAL_TIME_CHECK_ISR_ENABLED
#define MAX_INTERRUPT_LEVEL 4
typedef struct {
    uint32_t irq_number;
    uint32_t execution_time;
    void    *callback;          //recording the callback for debugging
} isr_time_check_t;
#endif /* HAL_TIME_CHECK_ISR_ENABLED */




#ifdef HAL_TIME_CHECK_ENABLED
void hal_time_check_enable(void);
void hal_time_check_disable(void);


#ifdef HAL_TIME_CHECK_ISR_ENABLED
void  time_check_start(uint32_t irq_number);
void time_check_end(uint32_t irq_number, uint32_t limter_us, void *callback);
#endif

#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
void hal_time_check_disable_irq_start(uint32_t linkAddress);
bool hal_time_check_disable_irq_end(uint32_t limter_us, uint32_t linkAddress);
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif /* HAL_TIME_CHECK_ENABLED*/
#endif /* __HAL_TIME_CHECK_H__ */

