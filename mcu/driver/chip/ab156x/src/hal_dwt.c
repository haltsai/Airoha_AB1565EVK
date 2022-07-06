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

#include "hal_dwt.h"

#ifdef HAL_DWT_MODULE_ENABLED
#include "hal_platform.h"
#include "core_cm4.h"
#include "hal_log.h"

static int DWT_NUMCOMP;

/* reset all comparators' setting */
void hal_dwt_reset(void)
{
    DWT->MASK0 = 0;
    DWT->MASK1 = 0;
    DWT->MASK2 = 0;
    DWT->MASK3 = 0;
    DWT->COMP0 = 0;
    DWT->COMP1 = 0;
    DWT->COMP2 = 0;
    DWT->COMP3 = 0;
    DWT->FUNCTION0 &= (~DWT_FUNCTION_FUNCTION_Msk);
    DWT->FUNCTION1 &= (~DWT_FUNCTION_FUNCTION_Msk);
    DWT->FUNCTION2 &= (~DWT_FUNCTION_FUNCTION_Msk);
    DWT->FUNCTION3 &= (~DWT_FUNCTION_FUNCTION_Msk);
}

void hal_dwt_init(void)
{
    /* only enable hardware stack overflow check by the DWT when halting debug is disabled,
           because under halting-mode, the ICE will take over the DWT function.
           So the software stack overflow need to be checked by SW under halting-mode.
           The halting debug status can be checked by the bit(C_DEBUGEND), which is set when debugger is connected.
      */

    if (!(CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
        /* DWT reset*/
        // hal_dwt_reset();

        /* enable debug monitor mode    */
        if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_MON_EN_Msk)) {
            CoreDebug->DEMCR |= (CoreDebug_DEMCR_MON_EN_Msk | CoreDebug_DEMCR_TRCENA_Msk) ;
        }

        DWT_NUMCOMP = DWT->CTRL >> DWT_CTRL_NUMCOMP_Pos;

#if 0
    hal_dwt_dump_status();
#endif /* HAL_DWT_DEBUG */
    }
}

void hal_dwt_dump_status(void)
{
    log_hal_msgid_info("DHCSR:0x%lx, DEMCR:0x%lx \r\n", 2, CoreDebug->DHCSR, CoreDebug->DEMCR);
    log_hal_msgid_info("DWT_CTRL: 0x%lx \r\n", 1, DWT->CTRL);
    log_hal_msgid_info("COMP0: %8lx \t MASK0: %8lx \t FUNC0: %8lx \r\n", 3, DWT->COMP0, DWT->MASK0, DWT->FUNCTION0);
    log_hal_msgid_info("COMP1: %8lx \t MASK1: %8lx \t FUNC1: %8lx \r\n", 3, DWT->COMP1, DWT->MASK1, DWT->FUNCTION1);
    log_hal_msgid_info("COMP2: %8lx \t MASK2: %8lx \t FUNC2: %8lx \r\n", 3, DWT->COMP2, DWT->MASK2, DWT->FUNCTION2);
    log_hal_msgid_info("COMP3: %8lx \t MASK3: %8lx \t FUNC3: %8lx \r\n", 3, DWT->COMP3, DWT->MASK3, DWT->FUNCTION3);
}

int32_t hal_dwt_request_watchpoint(hal_dwt_channel_t index, uint32_t addr_base, uint32_t addr_mask, hal_dwt_function_t func)
{
    uint32_t tmp;
    uint32_t *pcmpx = NULL, *pmaskx = NULL, *pfunx = NULL;
    /* only enable hardware stack overflow check by the DWT when halting debug is disabled,
           because under halting-mode, the ICE will take over the DWT function.
           The SW will do stack overflow under halting-mode.
           The halting debug status can be checked by the bit(C_DEBUGEND), which is set when debugger is connected.
       */
    if (!(CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
        /* comparator N */
        if ((index >= DWT_NUMCOMP) || (addr_base & ((1 << addr_mask) - 1))) {
            return -1;
        }

        /* Get the dwt channel */
        if (index == HAL_DWT_0) {
            pcmpx = (uint32_t *)(&DWT->COMP0);
            pmaskx = (uint32_t *)(&DWT->MASK0);
            pfunx = (uint32_t *)(&DWT->FUNCTION0);
        } else if (index == HAL_DWT_1) {
            pcmpx = (uint32_t *)(&DWT->COMP1);
            pmaskx = (uint32_t *)(&DWT->MASK1);
            pfunx = (uint32_t *)(&DWT->FUNCTION1);
        } else if (index == HAL_DWT_2) {
            pcmpx = (uint32_t *)(&DWT->COMP2);
            pmaskx = (uint32_t *)(&DWT->MASK2);
            pfunx = (uint32_t *)(&DWT->FUNCTION2);
        } else if (index == HAL_DWT_3) {
            pcmpx = (uint32_t *)(&DWT->COMP3);
            pmaskx = (uint32_t *)(&DWT->MASK3);
            pfunx = (uint32_t *)(&DWT->FUNCTION3);
        }

        if ((pcmpx != NULL) && (pmaskx != NULL) && (pfunx != NULL)) {
            /* set and enable comparator N */
            *pcmpx = addr_base;
            *pmaskx = addr_mask;
            tmp = *pfunx & DWT_FUNCTION_FUNCTION_Msk;
            *pfunx = tmp | func;

            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

int32_t hal_dwt_control_watchpoint(hal_dwt_channel_t index, hal_dwt_function_t func)
{
    uint32_t *pfunx = NULL;

    if (!(CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
        /* comparator N */
        if (index >= DWT_NUMCOMP) {
            return -1;
        } else {
            /* Get the dwt channel */
            if (index == HAL_DWT_0) {
                pfunx = (uint32_t *)(&DWT->FUNCTION0);
            } else if (index == HAL_DWT_1) {
                pfunx = (uint32_t *)(&DWT->FUNCTION1);
            } else if (index == HAL_DWT_2) {
                pfunx = (uint32_t *)(&DWT->FUNCTION2);
            } else if (index == HAL_DWT_3) {
                pfunx = (uint32_t *)(&DWT->FUNCTION3);
            }

            if (pfunx != NULL) {
                /* Update the setting */
                *pfunx = func & DWT_FUNCTION_FUNCTION_Msk;

                return 0;
            } else {
                return -1;
            }
        }
    } else {
        return -1;
    }
}

#endif /* HAL_DWT_MODULE_ENABLED */
