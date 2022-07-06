
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

/**
 * File: va_xiaowei_customer_config.h
 *
 * Description: This file defines the customer-defined items.
 *
 */


#ifndef __VA_XIAOWEI_CUSTOMER_CONFIG_H__
#define __VA_XIAOWEI_CUSTOMER_CONFIG_H__

#ifdef MTK_VA_XIAOWEI_ENABLE

#define VA_XIAOWEI_CUSTOMER_CONFIG_PRODUCT_ID   (830)       /* The product ID. */
#define VA_XIAOWEI_CUSTOMER_CONFIG_COMPANY_ID   (0x0094)    /* The company ID. */
#define VA_XIAOWEI_CUSTOMER_CONFIG_SKUID        (20)        /* The SKUID that generate form the Tencent cloud. */

/* The device name should be displayed in the Xiaowei Application main UI. */
#define VA_XIAOWEI_CUSTOMER_CONFIG_MODEL_NAME       "Airoha Xiaowei Test Model"

#define VA_XIAOWEI_CUSTOMER_CONFIG_CUSTOMER_NAME    "Airoha Xiaowei Test Device"    /* The customer name. */

#define VA_XIAOWEI_CUSTOMER_CONFIG_MODEL_NAME_LEN               strlen(VA_XIAOWEI_CUSTOMER_CONFIG_MODEL_NAME)
#define VA_XIAOWEI_CUSTOMER_CONFIG_CUSTOMER_NAME_LEN            strlen(VA_XIAOWEI_CUSTOMER_CONFIG_CUSTOMER_NAME)

#endif /* MTK_VA_XIAOWEI_ENABLE */

#endif /* __VA_XIAOWEI_CUSTOMER_CONFIG_H__ */
