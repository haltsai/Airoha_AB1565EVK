/*
 *  Minimal configuration for TLS 1.1 (RFC 4346)
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 * Minimal configuration for TLS 1.1 (RFC 4346), implementing only the
 * required ciphersuite: MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA
 *
 * See README.txt for usage instructions.
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_CALLOC_MACRO pvPortCalloc //mbedtls_calloc //
#define MBEDTLS_PLATFORM_FREE_MACRO	vPortFree //mbedtls_free //

#define MBEDTLS_SHA256_C

#define MBEDTLS_ENTROPY_HARDWARE_ALT

#ifdef CONFIG_MBEDTLS_HW_CRYPTO
#define MBEDTLS_AES_ALT
#define MBEDTLS_DES_ALT
#define MBEDTLS_MD5_ALT
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA512_ALT
#endif

#ifndef MTK_DEBUG_LEVEL_NONE
#define MBEDTLS_DEBUG_C
#endif

#define MBEDTLS_AES_C
// Tradeoff
// Uncomment this macro to use precomputed AES tables stored in ROM.
// -> potentially degraded performance if ROM access is slower than RAM access
// Comment this macro to generate AES tables in RAM at runtime.
// -> may stack overflow if your task stack size is lower than 4K bytes.
#define MBEDTLS_AES_ROM_TABLES

#define MBEDTLS_MD5_C

#define MBEDTLS_CHACHA20_C

#define MBEDTLS_MD_C

#ifdef AIR_SPOT_ENABLE
#define MBEDTLS_AES_ROM_TABLES
// ECP Feature
#define MBEDTLS_ECP_C

#define MBEDTLS_BASE64_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ENTROPY_C
// Use HW ENTROPY
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ECP_DP_SECP160R1_ENABLED

//#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#endif

/* MTK revisions */
#define MBEDTLS_MTK

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
