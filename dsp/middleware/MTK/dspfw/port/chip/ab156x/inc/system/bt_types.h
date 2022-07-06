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
#ifndef _BT_TYPES_H_
#define _BT_TYPES_H_

#include "bt_const.h"
#include "types.h"
#include "ctlr_config.h"
#include "bit.h"


////////////////////////////////////////////////////////////////////////////////
// Type Definitions ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* PBF
 * --
 * The data type defines the packet boundary flag */
typedef U8 PBF;
enum e_packet_boundary_identifier {
    PBF_FIRST_NON_AUTO_FLUSHABLE  = (PBF)(0UL),
    PBF_CONTINUING                = (PBF)(1UL),
    PBF_FIRST_AUTO_FLUSHABLE      = (PBF)(2UL),
};

/* LLID
 * --
 * The data type defines the logical link identifier */
typedef U8 LLID;
enum e_logical_link_identifier {
    LLID_RESERVED     = (LLID)(0UL),
    LLID_ACLU_CONT    = (LLID)(1UL),
    LLID_ACLU_START   = (LLID)(2UL),
    LLID_ACLC         = (LLID)(3UL)
};

/* Logical transport address (Logical Transport ADDRess)
 * --
 * The logical transport address is a 3-bit value. */
typedef U8 LTADDR;
typedef VOID* pLTADDR;



/* PTT (Packet Type Table)
 * --
 * The data type defines the packet type table */
typedef U8 PTT;
enum e_packet_type_table {
    PTT_ACLSCO_BASIC  = (PTT)(0UL),
    PTT_ESCO_BASIC    = (PTT)(1UL),
    PTT_ACLSCO_EDR    = (PTT)(2UL),
    PTT_ESCO_EDR      = (PTT)(3UL)
};


typedef U8 PNIDX;

enum enumeration_of_piconet_index {
  MASTER_PICONET_IDX              = (U8)0UL,
  SLAVE_PICONET0_IDX              = (U8)1UL,
  SLAVE_PICONET1_IDX              = (U8)2UL,
  SLAVE_PICONET2_IDX              = (U8)3UL,
};



enum enum_role {
  MASTER_ROLE = (0UL),
  SLAVE_ROLE  = (1UL)
};

/* Synchronization Word (SYNChronization WORD)
 * --
 * The sync word is one field in the bluetooth access code, which length is
 * 64-bit. */
typedef U8 SYNCWORD[8], *pSYNCWORD;



/* BCFLAG (BroadCast FLAG)
 * --
 * The data type defines the broadcast flag */
typedef U8 BCFLAG;
enum e_bc_flag {
  BCF_P2P             = (BCFLAG)(0UL),
  BCF_RX_NOT_IN_PARK  = (BCFLAG)(1UL),
  BCF_RX_IN_PARK      = (BCFLAG)(2UL)
};

/*
 * DHKey, Public Key
 */
typedef U8 DHKEY_ARR[24];
typedef U8 PUBLIC_KEY_P192_ARR[48];
typedef U8 PUBLIC_KEY_P256_ARR[64];
typedef U8 COMMIT_VALUE_ARR[16];
typedef U8 NONCE_VALUE_ARR[16];
typedef U8 CFM_VALUE_ARR[16];
typedef U8 SRES_ARR[4];

/*
 * Encription Key
 *
 * The maximun encription key size is 16. */
typedef U8 ENCKEY[16], *pENCKEY;

/* Initialization Vector
 * --
 * For encryption */
typedef U8 IV[8], *pIV;

typedef U8 ACCESSCODE[8];

/*
 * FHS Packet
 */
typedef struct stru_fhs_pkt
{
	/* Use unsigned int is better */
    U32  ParityBitLo     :32;
    U32  ParityBitHi     :2;
    U32  LapB0           :8;
    U32  LapB1           :8;
    U32  LapB2           :8;
    U32  Eir             :1;
    U32  Rsvd            :1;
    U32  SR              :2;
    U32  SP              :2;
    U32  Uap             :8;
    U32  NapB0           :8;
    U32  NapB1           :8;
    U32  CodB0           :8;
    U32  CodB1           :8;
    U32  CodB2           :8;
    U32  LtAddr          :3;
    U32  Clk             :26;
    U32  PageScanMode    :3;

} PACKED FHS_PKT_STRU, * FHS_PKT_STRU_PTR;

/*
 * Bluetooth Device Address
 */
typedef U8 LAP_ARR[3];
typedef U8 UAP;
typedef U8 NAP_ARR[2];

typedef struct stru_bd_addr
{
    LAP_ARR Lap;
    UAP Uap;
    NAP_ARR Nap;

} BD_ADDR_STRU, *pBDADDR;


//structure for resolvable address
typedef union
{
    struct PRAND_BIT_FIELD
    {
        U32 Rand:22;
        U32 MSO_2:2;
    }PACKED  BF;
    PACKED U32 Value:24;
}PRAND_STRU;


typedef struct
{
    U32 Hash:24;
    PRAND_STRU Prand;
}PACKED RPA_STRU, * pRPA_STRU;



typedef U8 PARITY_ARR[5];
typedef struct
{
    U8 PARITY_B0;
    U8 PARITY_B1;
    U8 PARITY_B2;
    U8 PARITY_B3;
    U8 PARITY_B4;
} PARITY_STRU, *pPARITY;

#define MAX_EIR_SIZE    240
typedef U8 EIR_ARR[MAX_EIR_SIZE];

/*
 * Bluetooth Device Name
 */
#define LOCAL_NAME_LEN      248
#define MAX_SUPP_NAME_LEN   32
/* tmp */
typedef struct stru_device_name
{
    U8 len;
    U8 name[MAX_SUPP_NAME_LEN];

} DEVICE_NAME_STRU;

/*
 * Pin Code
 */
#define PIN_TYPE_VARIABLE   0
#define PIN_TYPE_FIXED      1
typedef U8 PIN_CODE_ARR[16];

/*
 * Link Key or Long Term Key
 */
typedef U8 LINK_KEY_ARR[16];

/*
 * Key Information
 */
typedef struct stru_link_key_info
{
    U8 BdAddr[6];//BD_ADDR_STRU BdAddr;
    U8 LinkKey[16];//LINK_KEY_ARR LinkKey;

} LINK_KEY_INFO_STRU;

/*
 * Class Of Device
 */
typedef U8 COD_ARR[3];

/*
 * Bluetooth 3.0 Channel Map
 */
typedef U8 BT_CH_MAP_ARR[10];

typedef struct stru_bt3_channel_map
{
    BT_CH_MAP_ARR Field;

} BT3_CHANNEL_MAP_STRU, *pBT3CHMAP;


/*
 * Bluetooth 4.0 Channel Map
 */
typedef U8 LE_CH_MAP_ARR[5];

typedef U32 BTCLK;
typedef U16 BTPHASE;
typedef U8 BTCLK_ARR[4];

typedef enum {
    BT_CLK_Offset,
    ULL_CLK_Offset,
} BT_CLOCK_OFFSET_SCENARIO;

typedef struct stru_fine_bluetooth_clock
{
  U32 DoubleSlot;
  U16 Phase;
} FBTCLK, *pFBTCLK;


typedef struct stru_btclk
{
    U8 B3;
    U8 B2;
    U8 B1;
    U8 B0;

} BTCLK_STRU;

typedef union union_btclk
{
    BTCLK value;
    BTCLK_ARR byte;
    BTCLK_STRU field;

} BTCLK_UNION;

typedef struct stru_bttime
{
    BTCLK   period;
    BTPHASE phase;
} BTTIME_STRU, *BTTIME_STRU_PTR;


typedef union union_lc_le_adv_pdu_header_b0
{
    struct stru_lc_le_adv_pdu_header_b0
    {
        U8  PduType :4;
        U8  Rsvd    :1;
        U8  ChSel   :1;
        U8  TxAddr  :1;
        U8  RxAddr  :1;
    }Adv;

    struct stru_lc_le_data_pdu_header_b0
    {
        U8  Llid    :2;
        U8  Nesn    :1;
        U8  Sn      :1;
        U8  Md      :1;
        U8  Rsvd    :3;
    }Data;

    U8 value;

} LC_LE_PDU_HDR_B0_STRU, * LC_LE_PDU_HDR_B0_STRU_PTR;

typedef struct stru_lc_le_pdu_header_b1
{
    U8 length;

} LC_LE_PDU_HDR_B1_STRU, * LC_LE_PDU_HDR_B1_STRU_PTR;


typedef struct stru_llcp_hdr
{
    LC_LE_PDU_HDR_B0_STRU B0;
    LC_LE_PDU_HDR_B1_STRU B1;
} LLCP_HDR_STRU, *LLCP_HDR_STRU_PTR;




#define NUM_OF_HCI_SUPPORTED_COMMANDS    64
#define NUM_OF_STORED_INQ_RESULT_FILTER   2
#define NUM_OF_STORED_CONN_SETUP_FILTER   2

#define NUM_OF_SUPPORTED_IAC        ((U8)2)
#define NUM_OF_STORED_LINK_KEY      0x0001UL
#define D4_LINK_SUPERVISION_TIMEOUT 0x7D00UL


/* Bluetooth Clock Constant */
#define BTCLK_FULL_ROUND_TIME 0x10000000UL
#define BTCLK_HALF_ROUND_TIME 0x08000000UL
#define BTCLK_27_0_MASK 0x0FFFFFFFUL
#define BTPHASE_11_0_MASK 0x00000FFFUL

enum enum_lm_fea_mask
{
    LM_FEA_MASK_PAGE0,
    LM_FEA_MASK_PAGE1,
    LM_FEA_MASK_PAGE2,
    LM_FEA_MAX_PAGE_NUM = LM_FEA_MASK_PAGE2,

    NUM_OF_PAGE_OF_LMP_FEAS,
};

typedef BYTE_UNION SUPPORTTED_FEATURES[8];
typedef BYTE_UNION SUPPORTTED_STATES[8];

typedef struct stru_lmp_version_info
{
    U8 Version;
    U8 CompIdB0;
    U8 CompIdB1;
    U8 SubversionB0;
    U8 SubversionB1;

} VERSION_INFO;

typedef U8 COMP_ID[2];
typedef U8 LMP_SUBVERSION[2];

typedef U8 HCI_REVISION[2];


typedef U8 LE_ACCESS_ADDR[4];


typedef struct structure_of_le_access_address {
    U32 addr;
} LE_ACCADDR;

typedef U8 LE_ACCADDR_ARR[4];
typedef U8 LE_CRCINIT_ARR[3];
typedef U8 LE_CHNLMAP_ARR[5];

typedef U8 LE_IRK_ARR[16];

typedef U8 LE_RPA_ARR[6];

typedef U8 LE_PRIM_ADV_INT[3];

/* LE scan type (LE Scan Type)
 * --
 * For LE Scan */
typedef U8 LESCNTYP;
enum e_le_scan_type {
  LE_SCAN_PASSIVE             = (LESCNTYP)0,
  LE_SCAN_ACTIVE              = (LESCNTYP)1,
};

/* LE PDU type (LE PDU type)
 * --
 * The LE PDU type */
typedef U8 LEPDU;
enum e_le_pdu_type {
  LE_ADV_IND_TYPE             = (LEPDU)0,
  LE_ADV_DIRECT_IND_TYPE      = (LEPDU)1,
  LE_ADV_NONCONN_IND_TYPE     = (LEPDU)2,
  LE_SCAN_REQ_TYPE            = (LEPDU)3,
  LE_AUX_SCAN_REQ_TYPE        = (LEPDU)3,
  LE_SCAN_RSP_TYPE            = (LEPDU)4,
  LE_CONNECT_REQ_TYPE         = (LEPDU)5,
  LE_AUX_CONNECT_REQ_TYPE     = (LEPDU)5,
  LE_ADV_SCAN_IND_TYPE        = (LEPDU)6,
  LE_ADV_EXT_IND              = (LEPDU)7,
  LE_AUX_ADV_IND              = (LEPDU)7,
  LE_AUX_SCAN_RSP             = (LEPDU)7,
  LE_AUX_SYNC_IND             = (LEPDU)7,
  LE_AUX_CHAIN_IND            = (LEPDU)7,
  LE_AUX_CONN_RSP             = (LEPDU)8,
};



/* Advertising type (Adevertising type)
 * --
 * The Advertising type */
typedef U8 ADVTYPE;
enum e_le_advertising_Type
{
    ADV_IND                         = (ADVTYPE)0,
    ADV_DIRECT_IND_HIGH_DUTY_CYCLE  = (ADVTYPE)1,
    ADV_SCAN_IND                    = (ADVTYPE)2,
    ADV_NONCONN_IND                 = (ADVTYPE)3,
    ADV_DIRECT_IND_LOW_DUTY_CYCLE   = (ADVTYPE)4,
};

/* Legacy advertising event properties
 * --
 * The ADV event properties */
typedef U16 ADV_EVT_PROPS;
#define ADV_EVT_PROPS_ADV_IND                          (ADV_EVT_PROPS)0x13
#define ADV_EVT_PROPS_ADV_DIRECT_IND_LOW_DUTY_CYC      (ADV_EVT_PROPS)0x15
#define ADV_EVT_PROPS_ADV_DIRECT_IND_HIGH_DUTY_CYC     (ADV_EVT_PROPS)0x1D
#define ADV_EVT_PROPS_ADV_SCAN_IND                     (ADV_EVT_PROPS)0x12
#define ADV_EVT_PROPS_ADV_NONCONN_IND                  (ADV_EVT_PROPS)0x10

typedef union union_adv_evt_props
{
    struct stru_adv_evt_props
    {
        U16 ConnAdv                 :1;
        U16 ScanAdv                 :1;
        U16 DirectAdv               :1;
        U16 HighDutyCycDirectAdv    :1;
        U16 LegacyAdvPdu            :1;
        U16 AnonAdv                 :1;
        U16 IncTxPwr                :1;
        U16 Rsvd                    :9;
    } field;
    U16 value;

} PACKED ADV_EVT_PROPS_UNION;

/* LE PHY type (PHY type)
 * --
 * The LE PHY type */
typedef U8 PHYTYPE;
enum e_le_PHY_Type
{
    RSVD0                           = (PHYTYPE)0,
    LE_1M_PHY                       = (PHYTYPE)1,
    LE_2M_PHY                       = (PHYTYPE)2,
    LE_CODED_PHY                    = (PHYTYPE)3,
    LE_CODED_PHY_S8                 = (PHYTYPE)3,
    LE_CODED_PHY_S2                 = (PHYTYPE)4,
    NUM_OF_PHY_TYPE
};

/* LE Random Address Type (Random Address Type)
 * --
 * The LE Random Address Type */
typedef U8 RANDADDRTYPE;
enum e_le_Random_Address_Type
{
    RANDADDR_NON_RESOLVABLE     = (RANDADDRTYPE)0,
    RANDADDR_RESOLVABLE         = (RANDADDRTYPE)1,
    RANDADDR_STATIC             = (RANDADDRTYPE)3,
};


/* LE Address Type (Address Type)
 * --
 * The LE Address Type */
typedef U8 LEADDRTYPE;
enum e_le_Address_Type
{
    LEADDR_PUBLIC     = (LEADDRTYPE)0,
    LEADDR_RANDOM     = (LEADDRTYPE)1,
};

/* Advertising Filter Policy (Adevertising Filter Policy)
 * --
 * The Advertising Filter Policy */
typedef U8 ADVFLTRPLCY;
enum e_le_advertising_filter_policy
{
    ACCEPT_ALL        = (ADVFLTRPLCY)0,
    CHECK_SCAN_REQ    = (ADVFLTRPLCY)1,
    CHECK_CONN_REQ    = (ADVFLTRPLCY)2,
    CHECK_BOTH        = (ADVFLTRPLCY)3,
};

/* Extended Adv Mode (Adv Mode)
 * --
 * The Extended Adv Mode */
typedef U8 ADVMODE;
enum e_le_extended_adv_mode
{
    NONE          = (ADVMODE)0,
    CONNECTABLE   = (ADVMODE)1,
    SCANNABLE     = (ADVMODE)2,
};






/* LE White List */
#define LE_WHITE_LIST_SIZE        4 //- depends on HW
#define LE_RESOLVING_LIST_SIZE    3 //- depends on HW ,HW support 4 sets, but we take 1 for the non-privacy case

/* LE Adv Set */
#define NUM_OF_SUPP_LE_ADV_SETS   3

#if 0
////////////////////////////////////////////////////////////////////////////////
// Constant Defintions /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define SIZE_OF_ASIC_VERSION    10



////////////////////////////////////////////////////////////////////////////////
// Type Definitions ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef U8 ASIC_VERSION[SIZE_OF_ASIC_VERSION];



/* For Vender Command */
////////////////////////////////////////////////////////////////////////////////
// Type Definitions ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* AFE Init */
typedef struct stru_vcmd_afe_init
{
    U8 B0;
    U8 B1;
    U8 Addr;

} VCMD_AFE_INIT_STRU;

typedef struct stru_vcmd_sfr_init
{
    U8 Data;
    U8 Offset;
    U8 Bank;

} VCMD_SFR_INIT_STRU;

typedef struct stru_vcmd_delay_init
{
    U8 B0;
    U8 B1;
    U8 Tag;

} VCMD_DELAY_INIT_STRU;

typedef struct stru_vcmd_generic_init
{
    U8 B0;
    U8 B1;
    U8 B2;

} VCMD_GENERIC_INIT_STRU;

typedef union u_vcmd_init
{
    VCMD_AFE_INIT_STRU AfeInit;
    VCMD_SFR_INIT_STRU SfrInit;
    VCMD_DELAY_INIT_STRU DelayInit;
    VCMD_GENERIC_INIT_STRU GenericInit;

} VCMD_INIT_UNION;

/* SFR Structure */
typedef struct stru_vcmd_sfr
{
    U16 Addr;
    U8 ByteAlign;
    U8 Data[1];

} VCMD_SFR_STRU;

/* SFR Structure with Different Byte Align
 * The following variables are little endian */
typedef struct stru_vcmd_sfr_one_byte
{
    U16 Addr;
    U8 ByteAlign;
    U8 Data;

} VCMD_SFR_ONE_BYTE_STRU;

typedef struct stru_vcmd_sfr_two_byte
{
    U16 Addr;
    U8 ByteAlign;
    U16 Data;

} VCMD_SFR_TWO_BYTE_STRU;

typedef struct stru_vcmd_sfr_four_byte
{
    U16 Addr;
    U8 ByteAlign;
    U32 Data;

} VCMD_SFR_FOUR_BYTE_STRU;
#endif


#endif /* _BT_TYPES_H_ */

