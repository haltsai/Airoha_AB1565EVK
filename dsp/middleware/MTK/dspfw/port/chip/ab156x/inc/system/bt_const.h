/* Copyright Statement:
 *
 * (C) 2014  Airoha Technology Corp. All rights reserved.
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
#ifndef _BT_CONST_H_
#define _BT_CONST_H_

#include "types.h"



////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef U8 LMP_VERSION;
#define LMP_VERSION_OF_BT_CORE_SPEC_10B     ((LMP_VERSION)(0))
#define LMP_VERSION_OF_BT_CORE_SPEC_11      ((LMP_VERSION)(1))
#define LMP_VERSION_OF_BT_CORE_SPEC_12      ((LMP_VERSION)(2))
#define LMP_VERSION_OF_BT_CORE_SPEC_20EDR   ((LMP_VERSION)(3))
#define LMP_VERSION_OF_BT_CORE_SPEC_21EDR   ((LMP_VERSION)(4))
#define LMP_VERSION_OF_BT_CORE_SPEC_30HS    ((LMP_VERSION)(5))
#define LMP_VERSION_OF_BT_CORE_SPEC_40      ((LMP_VERSION)(6))
#define LMP_VERSION_OF_BT_CORE_SPEC_41      ((LMP_VERSION)(7))
#define LMP_VERSION_OF_BT_CORE_SPEC_42      ((LMP_VERSION)(8))
#define LMP_VERSION_OF_BT_CORE_SPEC_50      ((LMP_VERSION)(9))

typedef U8 HCI_VERSION;
#define HCI_VERSION_OF_BT_CORE_SPEC_10B     ((HCI_VERSION)(0))
#define HCI_VERSION_OF_BT_CORE_SPEC_11      ((HCI_VERSION)(1))
#define HCI_VERSION_OF_BT_CORE_SPEC_12      ((HCI_VERSION)(2))
#define HCI_VERSION_OF_BT_CORE_SPEC_20EDR   ((HCI_VERSION)(3))
#define HCI_VERSION_OF_BT_CORE_SPEC_21EDR   ((HCI_VERSION)(4))
#define HCI_VERSION_OF_BT_CORE_SPEC_30HS    ((HCI_VERSION)(5))
#define HCI_VERSION_OF_BT_CORE_SPEC_40      ((HCI_VERSION)(6))
#define HCI_VERSION_OF_BT_CORE_SPEC_41      ((HCI_VERSION)(7))
#define HCI_VERSION_OF_BT_CORE_SPEC_42      ((HCI_VERSION)(8))
#define HCI_VERSION_OF_BT_CORE_SPEC_50      ((HCI_VERSION)(9))

typedef U8 BT_PKT_CODE;
#define BT_PKT_CODE_NULL                     ((BT_PKT_CODE)0x0)
#define BT_PKT_CODE_POLL                     ((BT_PKT_CODE)0x1)
#define BT_PKT_CODE_FHS                      ((BT_PKT_CODE)0x2)
#define BT_PKT_CODE_DM1                      ((BT_PKT_CODE)0x3)
#define BT_PKT_CODE_DH1                      ((BT_PKT_CODE)0x4)
#define BT_PKT_CODE_2DH1                     ((BT_PKT_CODE)0x4)
#define BT_PKT_CODE_HV1                      ((BT_PKT_CODE)0x5)
#define BT_PKT_CODE_HV2                      ((BT_PKT_CODE)0x6)
#define BT_PKT_CODE_2EV3                     ((BT_PKT_CODE)0x6)
#define BT_PKT_CODE_HV3                      ((BT_PKT_CODE)0x7)
#define BT_PKT_CODE_EV3                      ((BT_PKT_CODE)0x7)
#define BT_PKT_CODE_3EV3                     ((BT_PKT_CODE)0x7)
#define BT_PKT_CODE_DV                       ((BT_PKT_CODE)0x8)
#define BT_PKT_CODE_3DH1                     ((BT_PKT_CODE)0x8)
#define BT_PKT_CODE_AUX1                     ((BT_PKT_CODE)0x9)
#define BT_PKT_CODE_DM3                      ((BT_PKT_CODE)0xA)
#define BT_PKT_CODE_2DH3                     ((BT_PKT_CODE)0xA)
#define BT_PKT_CODE_DH3                      ((BT_PKT_CODE)0xB)
#define BT_PKT_CODE_3DH3                     ((BT_PKT_CODE)0xB)
#define BT_PKT_CODE_EV4                      ((BT_PKT_CODE)0xC)
#define BT_PKT_CODE_2EV5                     ((BT_PKT_CODE)0xC)
#define BT_PKT_CODE_EV5                      ((BT_PKT_CODE)0xD)
#define BT_PKT_CODE_3EV5                     ((BT_PKT_CODE)0xD)
#define BT_PKT_CODE_DM5                      ((BT_PKT_CODE)0xE)
#define BT_PKT_CODE_2DH5                     ((BT_PKT_CODE)0xE)
#define BT_PKT_CODE_DH5                      ((BT_PKT_CODE)0xF)
#define BT_PKT_CODE_3DH5                     ((BT_PKT_CODE)0xF)

typedef U8 BT_LOGICAL_TRANSPORT;
#define BT_LOGICAL_TRANSPORT_ACL_SCO         ((BT_LOGICAL_TRANSPORT)0)
#define BT_LOGICAL_TRANSPORT_ESCO            ((BT_LOGICAL_TRANSPORT)1)

typedef U8 BT_DATA_RATE;
#define BT_DATA_RATE_BR                      ((BT_DATA_RATE)0)
#define BT_DATA_RATE_EDR                     ((BT_DATA_RATE)1)

typedef U8 BT_PKT_TYPE;
#define BT_COMBINED_PKT_TYPE(code,lt,rate)   (((BT_PKT_TYPE)(rate)<<5)|((BT_PKT_TYPE)(lt)<<4)|(BT_PKT_TYPE)code)

#define BT_PKT_TYPE_NULL                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_NULL,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_POLL                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_POLL,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_FHS                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_FHS,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_DM1                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_DM1,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_DH1                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_DH1,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_2DH1                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_2DH1,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_HV1                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_HV1,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_HV2                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_HV2,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_2EV3                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_2EV3,BT_LOGICAL_TRANSPORT_ESCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_HV3                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_HV3,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_EV3                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_EV3,BT_LOGICAL_TRANSPORT_ESCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_3EV3                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_3EV3,BT_LOGICAL_TRANSPORT_ESCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_DV                       BT_COMBINED_PKT_TYPE(BT_PKT_CODE_DV,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_3DH1                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_3DH1,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_AUX1                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_AUX1,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_DM3                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_DM3,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_2DH3                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_2DH3,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_DH3                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_DH3,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_3DH3                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_3DH3,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_EV4                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_EV4,BT_LOGICAL_TRANSPORT_ESCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_2EV5                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_2EV5,BT_LOGICAL_TRANSPORT_ESCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_EV5                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_EV5,BT_LOGICAL_TRANSPORT_ESCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_3EV5                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_3EV5,BT_LOGICAL_TRANSPORT_ESCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_DM5                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_DM5,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_2DH5                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_2DH5,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_EDR)
#define BT_PKT_TYPE_DH5                      BT_COMBINED_PKT_TYPE(BT_PKT_CODE_DH5,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_BR)
#define BT_PKT_TYPE_3DH5                     BT_COMBINED_PKT_TYPE(BT_PKT_CODE_3DH5,BT_LOGICAL_TRANSPORT_ACL_SCO,BT_DATA_RATE_EDR)

typedef struct stru_bt_pkt_type
{
    union
    {
        BT_PKT_TYPE Value;
        struct
        {
            BT_PKT_CODE TypeCode            :4;
            BT_LOGICAL_TRANSPORT LtType     :1;
            BT_DATA_RATE DataRate           :1;
            U8 Rsvd                         :2;
        } Field;
    };
} BT_PKT_TYPE_STRU;



#define BT_CRC_OK                       0x10
#define BT_HEC_OK                       0x08

#define BT_MAX_LENGTH_DM1               17
#define BT_MAX_LENGTH_DH1               27
#define BT_MAX_LENGTH_DM3               121
#define BT_MAX_LENGTH_DH3               183
#define BT_MAX_LENGTH_DM5               224
#define BT_MAX_LENGTH_DH5               339
#define BT_MAX_LENGTH_AUX1              29

#define BT_MAX_LENGTH_2DH1              54
#define BT_MAX_LENGTH_2DH3              367
#define BT_MAX_LENGTH_2DH5              679
#define BT_MAX_LENGTH_3DH1              83
#define BT_MAX_LENGTH_3DH3              552
#define BT_MAX_LENGTH_3DH5              1021

#define BT_MAX_LENGTH_HV1           10
#define BT_MAX_LENGTH_HV2           20
#define BT_MAX_LENGTH_HV3           30
#define BT_MAX_LENGTH_DV_VOICE      10
#define BT_MAX_LENGTH_DV_DATA       9
#define BT_MAX_LENGTH_DV_TOTAL      19
#define BT_MAX_LENGTH_EV3           30
#define BT_MAX_LENGTH_EV4           120
#define BT_MAX_LENGTH_EV5           180
#define BT_MAX_LENGTH_2EV3          60
#define BT_MAX_LENGTH_2EV5          360
#define BT_MAX_LENGTH_3EV3          90
#define BT_MAX_LENGTH_3EV5          540




#endif /* _BT_CONST_H_ */

