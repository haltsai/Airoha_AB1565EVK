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

#include "hal_aesotf.h"

#ifdef HAL_AESOTF_MODULE_ENABLED

#include <string.h>
#include "memory_attribute.h"
#include "hal_define.h"
#include "ab156x.h"
#include "hal_crypt_aesotf.h"
#include "hal_log.h"
#include "hal_crypt_internal.h"
#include "hal_nvic.h"
#include "hal_clock.h"

/* global lock used to protect the crypto engine */
int8_t g_aesotf_lock = AESOTF_UNLOCK;

/* internal function for getting lock, -100 means the crypto engine is not available */
int32_t hal_aesotf_lock_take(void)
{
    uint32_t irq_status;
    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    if (g_aesotf_lock == AESOTF_LOCK) {
        hal_nvic_restore_interrupt_mask(irq_status);
        return -100;
    }
    g_aesotf_lock = AESOTF_LOCK;
    hal_nvic_restore_interrupt_mask(irq_status);
    return 0;
}


/* internal function for releasing lock */
void hal_aesotf_lock_give(void)
{
    g_aesotf_lock = AESOTF_UNLOCK;
}


hal_aesotf_status_t hal_aesotf_init()
{
    hal_aesotf_status_t status = 0;

    if(hal_aesotf_lock_take()==HAL_AESOTF_STATUS_BUSY){
        return HAL_AESOTF_STATUS_BUSY;
    }

    hal_clock_enable(HAL_CLOCK_CG_AESOTF);
    status = aesotf_init();
    hal_clock_disable(HAL_CLOCK_CG_AESOTF);

    hal_aesotf_lock_give();
    return status;
}

hal_aesotf_status_t hal_aesotf_set_region(uint32_t begin_address, uint32_t end_address, uint8_t region)
{
    hal_aesotf_status_t status = 0;

    if(hal_aesotf_lock_take()==HAL_AESOTF_STATUS_BUSY){
        return HAL_AESOTF_STATUS_BUSY;
    }

    if (begin_address > end_address) {
        log_hal_msgid_error("invalid input that begin_address is bigger than end_address\r\n", 0);
        return HAL_AESOTF_STATUS_ERROR;
    }

    hal_clock_enable(HAL_CLOCK_CG_AESOTF);
    status = aesotf_set_region(begin_address, end_address, region);
    hal_clock_disable(HAL_CLOCK_CG_AESOTF);

    hal_aesotf_lock_give();
    return status;
}


hal_aesotf_status_t hal_aesotf_enable(hal_aesotf_buffer_t *key, uint8_t nonce[HAL_AESOTF_NONCE_LENGTH], uint8_t key_src)
{
    hal_aesotf_status_t status = 0;

    if(hal_aesotf_lock_take()==HAL_AESOTF_STATUS_BUSY){
        return HAL_AESOTF_STATUS_BUSY;
    }

    if (nonce == NULL) {
        log_hal_msgid_error("Invalid input of nonce\r\n", 0);
        return HAL_AESOTF_STATUS_ERROR;
    }
    aesotf_set_nonce(nonce);

    if (key_src >= AESOTF_KEY_SOURCE_NUM) {
        log_hal_msgid_error("Invalid input of key source\r\n", 0);
        return HAL_AESOTF_STATUS_ERROR;
    }
    aesotf_set_keysource(key_src);


    if (key == NULL && key_src == HAL_AESOTF_KEY_SOURCE_SWKEY) {
        log_hal_msgid_error("Invalid input of key \r\n", 0);
        return HAL_AESOTF_STATUS_ERROR;
    }
    if (key->length < HAL_AESOTF_KEY_LENGTH_128 && key_src == HAL_AESOTF_KEY_SOURCE_SWKEY) {
        log_hal_msgid_error("Invalid input of key length for SW key\r\n", 0);
        return HAL_AESOTF_STATUS_ERROR;
    }
    aesotf_set_key(key->length, key->buffer);

    hal_clock_enable(HAL_CLOCK_CG_AESOTF);
    status = aesotf_enable();

    hal_aesotf_lock_give();
    return status;
}


hal_aesotf_status_t hal_aesotf_disable()
{
    hal_aesotf_status_t status = 0;

    if(hal_aesotf_lock_take()==HAL_AESOTF_STATUS_BUSY){
        return HAL_AESOTF_STATUS_BUSY;
    }

    hal_clock_enable(HAL_CLOCK_CG_AESOTF);
    status = aesotf_disable();
    hal_clock_disable(HAL_CLOCK_CG_AESOTF);

    hal_aesotf_lock_give();
    return status;
}

#endif /* HAL_AESOTF_MODULE_ENABLED */

