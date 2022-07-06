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

#include "hal_aes.h"

#ifdef HAL_AES_MODULE_ENABLED

#include <string.h>
#include <stdio.h>
#include "memory_attribute.h"
#include "hal_define.h"
#include "ab156x.h"
#include "hal_crypt_aes.h"
#include "hal_log.h"
#include "hal_crypt_internal.h"
#include "hal_nvic.h"
#include "hal_clock.h"
#include "hal_cache.h"

/* global lock used to protect the crypto engine */
int8_t g_crypt_lock = CRYPT_UNLOCK;

/* below variables will be used for crypto hardware so must be placed in physical memory address*/
ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t *init_vector2;
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t last_block[HAL_AES_BLOCK_SIZES];
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t data_block[HAL_AES_BLOCK_SIZES];

/* internal function for getting lock, -100 means the crypto engine is not available */
int32_t hal_crypt_lock_take(void)
{
    uint32_t irq_status;
    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    if (g_crypt_lock == CRYPT_LOCK) {
        hal_nvic_restore_interrupt_mask(irq_status);
        return HAL_AES_STATUS_BUSY;
    }
    g_crypt_lock = CRYPT_LOCK;
    hal_nvic_restore_interrupt_mask(irq_status);
    return HAL_AES_STATUS_OK;
}


/* internal function for releasing lock */
void hal_crypt_lock_give(void)
{
    g_crypt_lock = CRYPT_UNLOCK;
}

volatile static bool g_aes_op_done = false;

/* It's about 10s at 192MHz CPU clock */
#define HAL_AES_MAX_WAIT_COUNT 0x10000000

void dump_crypto_register(void)
{
    log_hal_msgid_info("\r\n0xe000e100 =0x%x", 1, *((volatile unsigned int *) 0xE000E100));
    log_hal_msgid_info("\r\n0xe000e200 =0x%x", 1, *((volatile unsigned int *) 0xE000E200));
    log_hal_msgid_info("\r\n0xe000e300 =0x%x", 1, *((volatile unsigned int *) 0xE000E300));
}

static void aes_operation_done(hal_nvic_irq_t irq_number)
{
    uint32_t irq_status;

    NVIC_DisableIRQ((IRQn_Type)CRYPTO_IRQn);
    hal_nvic_save_and_set_interrupt_mask(&irq_status);

    g_aes_op_done = true;

    // clear interrupt status bit
    *CRYPTO_ENGINE_STA_BASE |= (1 << CRYPTO_ENGINE_STA_INT_CLR);
    NVIC_ClearPendingIRQ(CRYPTO_IRQn);

    hal_nvic_restore_interrupt_mask(irq_status);
    NVIC_EnableIRQ((IRQn_Type)CRYPTO_IRQn);
}

static hal_aes_status_t do_aes_encrypt(uint8_t *encrypt_buffer,
                                       uint32_t encrypt_buffer_length,
                                       uint8_t *plain_buffer,
                                       uint32_t plain_buffer_length,
                                       uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{

    g_aes_op_done = false;
    int32_t ret_val = aes_operate(encrypt_buffer,
                                  encrypt_buffer_length,
                                  plain_buffer,
                                  plain_buffer_length,
                                  init_vector,
                                  AES_MODE_ENCRYPT);
    if (ret_val < 0) {
        log_hal_msgid_error("aes_operate fail.", 0);
        return HAL_AES_STATUS_ERROR;
    }

#if 0
    uint32_t wait_count = 0;
    while (!g_aes_op_done) {
        //simple wait
        wait_count++;

        if (wait_count > HAL_AES_MAX_WAIT_COUNT) {
            log_hal_msgid_error("wait for encrypt timeout.", 0);
            return HAL_AES_STATUS_ERROR;
        }
    }

#else
    while ((*CRYPTO_ENGINE_CTRL_BASE) & (1 << ENGINE_CTRL_START_OFFSET));
    // clear interrupt status bit
    *CRYPTO_ENGINE_STA_BASE |= (1 << CRYPTO_ENGINE_STA_INT_CLR);
#endif
    return HAL_AES_STATUS_OK;
}

static hal_aes_status_t do_aes_decrypt(hal_aes_buffer_t *plain_text,
                                       hal_aes_buffer_t *encrypted_text,
                                       uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    g_aes_op_done = false;
    int32_t ret_val = aes_operate(encrypted_text->buffer,
                                  encrypted_text->length,
                                  plain_text->buffer,
                                  plain_text->length,
                                  init_vector,
                                  AES_MODE_DECRYPT);
    if (ret_val < 0) {
        log_hal_msgid_error("aes_operate fail.", 0);
        return HAL_AES_STATUS_ERROR;
    }

#if 0
    uint32_t wait_count = 0;
    while (!g_aes_op_done) {
        //simple wait
        wait_count++;
        if (wait_count > HAL_AES_MAX_WAIT_COUNT) {
            log_hal_msgid_error("wait for decrypt timeout.", 0);
            return HAL_AES_STATUS_ERROR;
        }
    }

#else
    while ((*CRYPTO_ENGINE_CTRL_BASE) & (1 << ENGINE_CTRL_START_OFFSET));

    // clear interrupt status bit
    *CRYPTO_ENGINE_STA_BASE |= (1 << CRYPTO_ENGINE_STA_INT_CLR);
#endif
    return HAL_AES_STATUS_OK;
}

/* internal common function */
hal_aes_status_t hal_aes_encrypt_with_padding(hal_aes_buffer_t *encrypted_text,
        hal_aes_buffer_t *plain_text,
        hal_aes_buffer_t *key,
        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
        uint8_t mode)
{
    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        log_hal_msgid_error("NULL input.", 0);
        return HAL_AES_STATUS_NULL_INPUT;
    }

    if ((plain_text->length % HAL_AES_BLOCK_SIZES) != 0) {
        log_hal_msgid_error("Invalid plain text length: %lu.", 1, plain_text->length);
        return HAL_AES_STATUS_INVALID_BLOCK_SIZE;
    }

    if (encrypted_text->length < plain_text->length) {
        log_hal_msgid_error("Inadequate encrypted buffer.", 0);
        return HAL_AES_STATUS_NOT_ENOUGH_OUTPUT_BUFFER;
    }
    if ((key->length != HAL_AES_KEY_LENGTH_128)
            && (key->length != HAL_AES_KEY_LENGTH_192)
            && (key->length != HAL_AES_KEY_LENGTH_256)) {
        log_hal_msgid_error("key length is %lu, invalid. It has to be 16, 24 or 32.", 1, key->length);
        return HAL_AES_STATUS_INVALID_KEY_LENGTH;
    }

    if (hal_cache_is_cacheable((uint32_t)(plain_text->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }
    if (hal_cache_is_cacheable((uint32_t)(encrypted_text->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }

    hal_nvic_register_isr_handler((IRQn_Type)CRYPTO_IRQn, aes_operation_done);
    NVIC_EnableIRQ((IRQn_Type)CRYPTO_IRQn);

    aes_set_key((uint8_t)key->length, key->buffer);
    aes_configure(AES_KEY_SOURCE_SW, mode);

    if (HAL_AES_STATUS_OK != do_aes_encrypt(encrypted_text->buffer,
                                            encrypted_text->length,
                                            plain_text->buffer,
                                            plain_text->length,
                                            init_vector)) {
        log_hal_msgid_error("do_aes_encrypt fail.", 0);
        return HAL_AES_STATUS_ENCRYPT_FAIL;
    }

    encrypted_text->length = plain_text->length;

    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_decrypt(hal_aes_buffer_t *plain_text,
                                 hal_aes_buffer_t *encrypted_text,
                                 hal_aes_buffer_t *key,
                                 uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                 uint8_t mode)
{
    if ((NULL == plain_text)
            || (NULL == encrypted_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        log_hal_msgid_error("NULL input.", 0);
        return HAL_AES_STATUS_NULL_INPUT;
    }

    if ((encrypted_text->length % HAL_AES_BLOCK_SIZES) != 0) {
        log_hal_msgid_error("Invalid encrypted text length: %lu.", 1, encrypted_text->length);
        return HAL_AES_STATUS_INVALID_BLOCK_SIZE;
    }

    if (plain_text->length < encrypted_text->length) {
        log_hal_msgid_error("Plain text buffer lengthL %lu is too small, encrypted length is: %lu", 2,
                      plain_text->length, encrypted_text->length);
        return HAL_AES_STATUS_NOT_ENOUGH_OUTPUT_BUFFER;
    }

    if ((key->length != HAL_AES_KEY_LENGTH_128)
            && (key->length != HAL_AES_KEY_LENGTH_192)
            && (key->length != HAL_AES_KEY_LENGTH_256)) {
        log_hal_msgid_error("key length is %lu, invalid. It has to be 16, 24 or 32.", 1, key->length);
        return HAL_AES_STATUS_INVALID_KEY_LENGTH;
    }

    if (hal_cache_is_cacheable((uint32_t)(plain_text->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }

    if (hal_cache_is_cacheable((uint32_t)(encrypted_text->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }

    hal_nvic_register_isr_handler((IRQn_Type)CRYPTO_IRQn, aes_operation_done);
    NVIC_EnableIRQ((IRQn_Type)CRYPTO_IRQn);

    aes_set_key(key->length, key->buffer);
    aes_configure(AES_KEY_SOURCE_SW, mode);

    if (HAL_AES_STATUS_OK != do_aes_decrypt(plain_text, encrypted_text, init_vector)) {
        log_hal_msgid_error("do_aes_decrypt fail", 0);
        return HAL_AES_STATUS_ERROR;
    }

    if (mode == AES_TYPE_CBC)
    {
         aes_update_iv(init_vector);
    }
    plain_text->length = encrypted_text->length;

    return HAL_AES_STATUS_OK;
}


/* internal common function */
hal_aes_status_t hal_aes_encrypt_with_padding_ex(hal_aes_buffer_t *encrypted_text,
        hal_aes_buffer_t *plain_text,
        hal_aes_buffer_t *key,
        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
        uint8_t mode,
        uint8_t key_src)
{
    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        log_hal_msgid_error("NULL input.", 0);
        return HAL_AES_STATUS_NULL_INPUT;
    }

    if ((plain_text->length % HAL_AES_BLOCK_SIZES) != 0) {
        log_hal_msgid_error("Invalid plain text length: %lu.", 1, plain_text->length);
        return HAL_AES_STATUS_INVALID_BLOCK_SIZE;
    }

    if (encrypted_text->length < plain_text->length) {
        log_hal_msgid_error("Inadequate encrypted buffer.", 0);
        return HAL_AES_STATUS_NOT_ENOUGH_OUTPUT_BUFFER;
    }

    if ((key->length != HAL_AES_KEY_LENGTH_128)
      &&(key->length != HAL_AES_KEY_LENGTH_192)
      &&(key->length != HAL_AES_KEY_LENGTH_256)) {
        log_hal_msgid_error("key length is %lu, invalid. It has to be 16, 24 or 32.", 1, key->length);
        return HAL_AES_STATUS_INVALID_KEY_LENGTH;
    }

    if (hal_cache_is_cacheable((uint32_t)(plain_text->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }
    if (hal_cache_is_cacheable((uint32_t)(encrypted_text->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }

    hal_nvic_register_isr_handler((IRQn_Type)CRYPTO_IRQn, aes_operation_done);
    NVIC_EnableIRQ((IRQn_Type)CRYPTO_IRQn);

    aes_set_key((uint8_t)key->length, key->buffer);

    // Set the key source and aes operation mode
    if(HAL_AES_KEY_SOURCE_EFUSE1 == key_src){
        aes_configure(AES_KEY_SOURCE_EFUSE1, mode);
    } else if (HAL_AES_KEY_SOURCE_EFUSE2 == key_src){
        aes_configure(AES_KEY_SOURCE_EFUSE2, mode);
    } else if (HAL_AES_KEY_SOURCE_CKDF1 == key_src){
        return HAL_AES_STATUS_INVALID_KEY_SOURCE;
    } else if (HAL_AES_KEY_SOURCE_CKDF2 == key_src){
        aes_configure(AES_KEY_SOURCE_CKDF2, mode);
    } else {
        return HAL_AES_STATUS_INVALID_KEY_SOURCE;
    }


    if (HAL_AES_STATUS_OK != do_aes_encrypt(encrypted_text->buffer,
                                            encrypted_text->length,
                                            plain_text->buffer,
                                            plain_text->length,
                                            init_vector)) {
        log_hal_msgid_error("do_aes_encrypt fail.", 0);
        return HAL_AES_STATUS_ENCRYPT_FAIL;
    }

    encrypted_text->length = plain_text->length;

    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_decrypt_ex(hal_aes_buffer_t *plain_text,
                                 hal_aes_buffer_t *encrypted_text,
                                 hal_aes_buffer_t *key,
                                 uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                 uint8_t mode,
                                 uint8_t key_src)
{
    if ((NULL == plain_text)
      ||(NULL == encrypted_text)
      ||(NULL == key)
      ||(NULL == init_vector)) {
        log_hal_msgid_error("NULL input.", 0);
        return HAL_AES_STATUS_NULL_INPUT;
    }

    if ((encrypted_text->length % HAL_AES_BLOCK_SIZES) != 0) {
        log_hal_msgid_error("Invalid encrypted text length: %lu.", 1, encrypted_text->length);
        return HAL_AES_STATUS_INVALID_BLOCK_SIZE;
    }

    if (plain_text->length < encrypted_text->length) {
        log_hal_msgid_error("Plain text buffer lengthL %lu is too small, encrypted length is: %lu", 2,
                      plain_text->length, encrypted_text->length);
        return HAL_AES_STATUS_NOT_ENOUGH_OUTPUT_BUFFER;
    }

    if ((key->length != HAL_AES_KEY_LENGTH_128)
            && (key->length != HAL_AES_KEY_LENGTH_192)
            && (key->length != HAL_AES_KEY_LENGTH_256)) {
        log_hal_msgid_error("key length is %lu, invalid. It has to be 16, 24 or 32.", 1, key->length);
        return HAL_AES_STATUS_INVALID_KEY_LENGTH;
    }

    if (hal_cache_is_cacheable((uint32_t)(plain_text->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }
    if (hal_cache_is_cacheable((uint32_t)(encrypted_text->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }

    hal_nvic_register_isr_handler((IRQn_Type)CRYPTO_IRQn, aes_operation_done);
    NVIC_EnableIRQ((IRQn_Type)CRYPTO_IRQn);

    aes_set_key(key->length, key->buffer);

    // Set the key source and aes operation mode
    if(HAL_AES_KEY_SOURCE_EFUSE1 == key_src){
        aes_configure(AES_KEY_SOURCE_EFUSE1, mode);
    } else if (HAL_AES_KEY_SOURCE_EFUSE2 == key_src){
        aes_configure(AES_KEY_SOURCE_EFUSE2, mode);
    } else if (HAL_AES_KEY_SOURCE_CKDF1 == key_src){
        return HAL_AES_STATUS_INVALID_KEY_SOURCE;
    } else if (HAL_AES_KEY_SOURCE_CKDF2 == key_src){
        aes_configure(AES_KEY_SOURCE_CKDF2, mode);
    } else {
        return HAL_AES_STATUS_INVALID_KEY_SOURCE;
    }

    if (HAL_AES_STATUS_OK != do_aes_decrypt(plain_text, encrypted_text, init_vector)) {
        log_hal_msgid_error("do_aes_decrypt fail", 0);
        return HAL_AES_STATUS_DECRYPT_FAIL;
    }

    if (mode == AES_TYPE_CBC)
    {
         aes_update_iv(init_vector);
    }
    plain_text->length = encrypted_text->length;

    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_cbc_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    hal_aes_status_t status;

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    status = hal_aes_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, AES_TYPE_CBC);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_cbc_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    hal_aes_status_t status;

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    status = hal_aes_decrypt(plain_text, encrypted_text, key, init_vector, AES_TYPE_CBC);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_ecb_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key)
{
    uint8_t init_vector[16] = {0};
    hal_aes_status_t status;

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    status = hal_aes_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, AES_TYPE_ECB);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_ecb_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key)
{
    uint8_t init_vector[HAL_AES_CBC_IV_LENGTH] = {0};
    hal_aes_status_t status;

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    status = hal_aes_decrypt(plain_text, encrypted_text, key, init_vector, AES_TYPE_ECB);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_cbc_encrypt_ex(hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *key,
                                        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                        hal_aes_key_src  key_src)
{
    hal_aes_status_t status;

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    status = hal_aes_encrypt_with_padding_ex(encrypted_text, plain_text, key, init_vector, AES_TYPE_CBC, key_src);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_cbc_decrypt_ex(hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *key,
                                        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                        hal_aes_key_src  key_src)
{
    hal_aes_status_t status;

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    status = hal_aes_decrypt_ex(plain_text, encrypted_text, key, init_vector, AES_TYPE_CBC, key_src);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_ecb_encrypt_ex(hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *key,
                                        hal_aes_key_src  key_src)
{
    uint8_t init_vector[HAL_AES_CBC_IV_LENGTH] = {0};
    hal_aes_status_t status;

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    status = hal_aes_encrypt_with_padding_ex(encrypted_text, plain_text, key, init_vector, AES_TYPE_ECB, key_src);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_ecb_decrypt_ex(hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *key,
                                        hal_aes_key_src  key_src)
{
    uint8_t init_vector[HAL_AES_CBC_IV_LENGTH] = {0};
    hal_aes_status_t status;

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    status = hal_aes_decrypt_ex(plain_text, encrypted_text, key, init_vector, AES_TYPE_ECB, key_src);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_set_ckdf(uint32_t *SWOTP, int8_t Lock_SWOTP, int8_t Lock_Key, int8_t Slot)
{
    hal_aes_status_t status = 0;

    if (SWOTP == NULL) {
        log_hal_msgid_error("invalid SWOTP\r\n", 0);
        return HAL_AES_STATUS_ERROR;
    }

    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);

    aes_set_ckdf(SWOTP, Lock_SWOTP, Lock_Key);

    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return status;
}

hal_aes_status_t hal_aes_cmac_message_hw(uint8_t* cmac_tag,
                                      hal_aes_buffer_t *message,
                                      uint32_t message_len,
                                      hal_aes_buffer_t *key,
                                      hal_aes_key_src  key_src)
{
//    hal_aes_status_t status;
    uint8_t init_vector[HAL_AES_CBC_IV_LENGTH] = {0};
    uint32_t         block_num  = 0;
    uint32_t         lastb_size = 0;
    uint8_t          *padding_start;
    uint32_t         idx = 0;
    uint8_t          key_in = 0;
    hal_aes_buffer_t tmp_enc;

    if ((NULL == message)
      ||(NULL == cmac_tag)
      ||(NULL == key)
      || message_len == 0) {
        log_hal_msgid_error("NULL input.", 0);
        return HAL_AES_STATUS_NULL_INPUT;
    }

    if ((message->length % HAL_AES_BLOCK_SIZES)
        || (message->length < message_len)) {
        return HAL_AES_STATUS_INVALID_BLOCK_SIZE;
    }

    if (hal_cache_is_cacheable((uint32_t)(message->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }
    if (hal_cache_is_cacheable((uint32_t)(key->buffer))) {
        return HAL_AES_STATUS_ERROR;
    }
    if (hal_cache_is_cacheable((uint32_t)(cmac_tag))) {
        return HAL_AES_STATUS_ERROR;
    }
    if (hal_crypt_lock_take() == HAL_AES_STATUS_BUSY) {
        return HAL_AES_STATUS_BUSY;
    }
    memset(last_block, 0x0, sizeof(last_block));
    memset(data_block, 0x0, sizeof(data_block));
    memset(&tmp_enc, 0x0, sizeof(hal_aes_buffer_t));

    hal_clock_enable(HAL_CLOCK_CG_CRYPTO);
    aes_set_key((uint8_t)key->length, key->buffer);

        // Set the key source and aes operation mode
    if (HAL_AES_KEY_SOURCE_SW_KEY == key_src) {
        key_in = AES_KEY_SOURCE_SW;
    } else if (HAL_AES_KEY_SOURCE_EFUSE1 == key_src) {
        key_in = AES_KEY_SOURCE_EFUSE1;
    } else if (HAL_AES_KEY_SOURCE_EFUSE2 == key_src) {
        key_in = AES_KEY_SOURCE_EFUSE2;
    } else if (HAL_AES_KEY_SOURCE_CKDF2 == key_src) {
        key_in = AES_KEY_SOURCE_CKDF2;
    } else {
        hal_crypt_lock_give();
        return HAL_AES_STATUS_INVALID_KEY_SOURCE;
    }

    block_num = (message_len + HAL_AES_BLOCK_SIZES -1) / HAL_AES_BLOCK_SIZES;
    lastb_size = message_len % HAL_AES_BLOCK_SIZES;

    if (lastb_size) {

        tmp_enc.buffer = data_block;
        if (block_num > 1 && ( (HAL_AES_STATUS_OK != aes_configure(key_in, AES_TYPE_CBC))
                              || (HAL_AES_STATUS_OK != do_aes_encrypt(tmp_enc.buffer,
                                                tmp_enc.length,
                                                message->buffer,
                                                message_len - lastb_size,
                                                init_vector)))) {
            log_hal_msgid_error("do_aes_encrypt fail.", 0);
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ENCRYPT_FAIL;
        }

        memcpy(last_block, message->buffer + message_len - lastb_size, lastb_size);
        padding_start = last_block + lastb_size;
        *padding_start = 0x80;
        for (idx = 0; idx < HAL_AES_BLOCK_SIZES; idx++) {
             last_block[idx] = tmp_enc.buffer[idx]^last_block[idx];
        }
        aes_configure(key_in, AES_TYPE_CMAC);
        cmac_operate(cmac_tag, last_block, HAL_AES_BLOCK_SIZES, 1);

    }
    else {
        aes_configure(key_in, AES_TYPE_CMAC);
        cmac_operate(cmac_tag, message->buffer, HAL_AES_BLOCK_SIZES * block_num, 0);
    }
    while((*CRYPTO_ENGINE_CTRL_BASE) & (1 << ENGINE_CTRL_START_OFFSET));
    // clear interrupt status bit
    *CRYPTO_ENGINE_STA_BASE |= (1 << CRYPTO_ENGINE_STA_INT_CLR);
    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_crypt_lock_give();
    return HAL_AES_STATUS_OK;
}

#endif /* HAL_AES_MODULE_ENABLED */

