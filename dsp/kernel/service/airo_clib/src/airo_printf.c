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
 
#ifdef MTK_CLIB_PRINTF_ENABLE

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define INT_MAX ((int)(~0U>>1))

#define BUFFER_OVERFLOW_CHECK(i, size, buf, space) \
    do {    \
        if ((size < (space+1)) || (i > (size - 1 - space))) {  \
            buf[i] = 0; \
            return (space + i + 1);   \
        }   \
    } while(0)

static uint32_t uint_to_str(uint32_t number, uint8_t *str_array)
{
    uint32_t i, value, bits;
    uint8_t byte_array[16];

    bits = 0;
    value = number;
    do {
        byte_array[bits++] = value % 10;
        value /= 10;
    } while (value);

    for (i = 0; i < bits; i++) {
        str_array[i] = byte_array[bits - i - 1] + '0';
    }

    return bits;
}

static uint32_t int_to_str(int32_t number, uint8_t *str_array)
{
    uint32_t i, j, value, bits;
    uint8_t byte_array[16];

    j = 0;
    value = number;
    if (number < 0) {
        str_array[j++] = '-';
        value = (~number) + 1;
    }

    bits = 0;
    do {
        byte_array[bits++] = value % 10;
        value /= 10;
    } while (value);

    for (i = 0; i < bits; i++) {
        str_array[j++] = byte_array[bits - i - 1] + '0';
    }

    return j;
}

static uint32_t hex_to_str(uint32_t number, uint8_t *str_array)
{
    uint32_t i, value, bits;
    uint8_t byte_array[16], curr_byte;

    bits = 0;
    value = number;
    do {
        byte_array[bits++] = value % 16;
        value /= 16;
    } while (value);

    for (i = 0; i < bits; i++) {
        curr_byte = byte_array[bits - i - 1];
        if (curr_byte >= 10) {
            str_array[i] = curr_byte - 10 + 'A';
        } else {
            str_array[i] = curr_byte + '0';
        }
    }

    return bits;
}

int32_t __wrap_vsnprintf(char *buf, uint32_t size, const char *fmt, va_list args)
{
    int32_t int_word;
    uint32_t i, bytes, uint_word;
    char *str, *end;
    uint8_t hex_array[16];

    if (size > INT_MAX) {
        return 0;
    }

    str = buf;
    end = buf + size;
    if (end < buf) {
        end = ((void *) - 1);
        size = end - buf;
    }

    i = 0;
    while (*fmt) {
        if (*fmt != '%') {
            BUFFER_OVERFLOW_CHECK(i, size, buf, 1);
            buf[i++] = *fmt++;
            continue;
        }
        fmt++;
        switch (*fmt) {
            case 'c':
                BUFFER_OVERFLOW_CHECK(i, size, buf, 1);
                buf[i++] = va_arg(args, int32_t);
                break;
            case 'd':
                int_word = va_arg(args, int32_t);
                bytes = int_to_str(int_word, hex_array);
                BUFFER_OVERFLOW_CHECK(i, size, buf, bytes);
                memcpy(&buf[i], hex_array, bytes);
                i += bytes;
                break;
            case 'u':
                uint_word = va_arg(args, uint32_t);
                bytes = uint_to_str(uint_word, hex_array);
                BUFFER_OVERFLOW_CHECK(i, size, buf, bytes);
                memcpy(&buf[i], hex_array, bytes);
                i += bytes;
                break;
            case 's':
                str = va_arg(args, char *);
                bytes = strlen(str);
                BUFFER_OVERFLOW_CHECK(i, size, buf, bytes);
                strncpy(&buf[i], str, bytes);
                i += bytes;
                break;
            case 'x':
                uint_word = va_arg(args, uint32_t);
                bytes = hex_to_str(uint_word, hex_array);
                BUFFER_OVERFLOW_CHECK(i, size, buf, bytes);
                memcpy(&buf[i], hex_array, bytes);
                i += bytes;
                break;
        }
        fmt++;
        *(buf+i) = '\0';
    }

    return i;
}

int32_t __wrap_snprintf(char *buf, uint32_t size, const char *fmt, ...)
{
    va_list args;
    int32_t i;

    va_start(args, fmt);
    i = __wrap_vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}

#endif

