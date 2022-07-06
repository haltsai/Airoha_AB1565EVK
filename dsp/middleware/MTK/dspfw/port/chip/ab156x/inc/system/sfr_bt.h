/* Copyright Statement:
 *
 * (C) 2016  Airoha Technology Corp. All rights reserved.
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
#ifndef __SFR_BT_H__
#define __SFR_BT_H__

#include "bt_types.h"

#define BT_RELATED_REG_BASE_ADDR 0x62860000

/******************************************************************************
 * Type Definition
 ******************************************************************************/


/* Test control table (Test Control Table)
 * --
 * The data type defines the test control table which is placed in baseband
 * registers. */


typedef struct s_bt3_test_ctl {
    U8 Type       : 4 ;
    U8 EscoInd    : 1 ;
    U8 TxDataRate : 1 ;
    U8 RxDataRate : 1 ;
    U8 Rsvd       : 1 ;
} TESTCTL3, *pTESTCTL3;

typedef struct s_test_control_table {
    U8  rTestEnable;                              /* Offset 0x44C */
    U8  rTestWhitenEnable;                        /* Offset 0x44D */
    U16 _reserved_word_44Eh;                      /* Offset 0x44E */

    TESTCTL3 rTestCtl;                            /* Offset 0x450 */

    U8  _reserved_byte_451h;                      /* Offset 0x451 */
    U16 rTestBt3EscoLen;                          /* Offset 0x452 */
    U8  rTestFixedChannelEnable;                  /* Offset 0x454 */
    U8  _reserved_byte_455h;                      /* Offset 0x455 */
    U8  rTestTxChannel;                           /* Offset 0x456 */
    U8  rTestRxChannel;                           /* Offset 0x457 */
} rTCTAB, *pTCTAB;



typedef struct s_aux_test_mode_control {
    pTCTAB   pTestCtlTab;
    U16      DataLen;
    U8       PktType;
    U8       Pattern;
    VOID*    pRxPtr;

} ATCTL, *pATCTL;


typedef struct RFON_CTL_s
{
	U16 sx_tx_settle_time;
	U8  tx_settle_time;
	U8  tx_path_delay;
	U16 sx_rx_settle_time;
	U8  rx_settle_time;
	U8  rx_path_delay;

} RFON_CTL;

typedef struct
{
    U8 BdAddr[6];
    U8 AddrType;
    U8 Rsvd;
}LE_ADDR_REG_STRU, * LE_ADDR_REG_STRU_PTR;


typedef struct
{
    BD_ADDR_STRU      rBdAddr;                /* Offset 0x188 */  /* Offset 0x00 */
    U16               _rsvd_word_06h;         /* Offset 0x18E */  /* Offset 0x06 */
    PARITY_STRU       rParity;                /* Offset 0x190 */  /* Offset 0x08 */
    U8                _rsvd_byte_0Dh;         /* Offset 0x195 */  /* Offset 0x0D */
    U16               _rsvd_word_0Eh;         /* Offset 0x196 */  /* Offset 0x0E */
    U32               rCod;                   /* Offset 0x198 */  /* Offset 0x10 */

}LOCAL_DEVICE_CONFIG_STRU, * LOCAL_DEVICE_CONFIG_STRU_PTR;


typedef struct
{
    RFON_CTL bt3_rfon_ctl;                   /* Offset 0x124 */  /* Offset 0x80 */
    RFON_CTL ble_rfon_ctl;                   /* Offset 0x12C */  /* Offset 0x88 */
    U8       pa_settle_time;                 /* Offset 0x134 */  /* Offset 0x90 */
    U8       _reserved_byte_91h;             /* Offset 0x135 */  /* Offset 0x91 */
    U16      _reserved_word_92h;             /* Offset 0x136 */  /* Offset 0x92 */
    U8       tx_mod_settle_time;             /* Offset 0x138 */  /* Offset 0x98 */
    U8       rx_mod_settle_time;             /* Offset 0x139 */  /* Offset 0x99 */
    U16      _reserved_word_9Ah;             /* Offset 0x13A */  /* Offset 0x9A */
    U8       rf_tx_off_delay;                /* Offset 0x13C */  /* Offset 0x9C */
    U8       sx_tx_off_delay;                /* Offset 0x13D */  /* Offset 0x9D */
    U16      _reserved_word_9Eh;             /* Offset 0x13E */  /* Offset 0x9E */
    U8       pa_off_delay;                   /* Offset 0x140 */  /* Offset 0xA0 */
    U8       _reserved_byte_A1h;             /* Offset 0x141 */  /* Offset 0xA1 */
    U16      _reserved_word_A2h;             /* Offset 0x142 */  /* Offset 0xA2 */
    U8       br_edr_mod_tx_off_delay;        /* Offset 0x144 */  /* Offset 0xA4 */
    U8       ble_mod_tx_off_delay;           /* Offset 0x145 */  /* Offset 0xA5 */
    U8       br_edr_mod_rx_off_delay;        /* Offset 0x146 */  /* Offset 0xA6 */
    U8       ble_mod_rx_off_delay;           /* Offset 0x147 */  /* Offset 0xA7 */
    U8       bt3_bs_tx_edr_guard_time;       /* Offset 0x148 */  /* Offset 0xA8 */
    U8       _reserved_byte_A9h;             /* Offset 0x149 */  /* Offset 0xA9 */
    U16      _reserved_word_AAh;             /* Offset 0x14A */  /* Offset 0xAA */
    U8       bt3_e0_delay;                   /* Offset 0x14C */  /* Offset 0xAC */
    U8       _reserved_byte_ADh;             /* Offset 0x14D */  /* Offset 0xAD */
    U16      _reserved_word_AEh;             /* Offset 0x14E */  /* Offset 0xAE */
    U16      ble_tx_hop_delay;               /* Offset 0x150 */  /* Offset 0xB0 */
    U16      ble_rx_hop_delay;               /* Offset 0x152 */  /* Offset 0xB2 */
    U16      ble_srch_win_center_2m;         /* Offset 0x154 */  /* Offset 0xB4 */
    U8       ble_rx_path_delay_2m;           /* Offset 0x156 */  /* Offset 0xB6 */
    U8       ble_tx_path_delay_2m;           /* Offset 0x157 */  /* Offset 0xB7 */
    U8       bt3_rx_ac_abort_time;           /* Offset 0x158 */  /* Offset 0xB8 */
    U8       _reserved_byte_B9h;             /* Offset 0x159 */  /* Offset 0xB9 */
    U16      _reserved_word_BAh;             /* Offset 0x15A */  /* Offset 0xBA */
    U16      bt3_tx_hop_delay;               /* Offset 0x15C */  /* Offset 0xBC */
    U16      bt3_rx_hop_delay;               /* Offset 0x15E */  /* Offset 0xBE */

}MODEM_CTL_STRU, * MODEM_CTL_STRU_PTR;

typedef struct
{
    BTCLK     rNativeClock;                   /*Offset 0x104*/ /* Offset 0xEC */
    U16       rNativePhase;                   /*Offset 0x108*/ /* Offset 0xF0 */
    U8        rNativeClkCtl;                  /*Offset 0x10A*/ /* Offset 0xF2 */
    U8        _reserved_byte_F3h;             /*Offset 0x10B*/ /* Offset 0xF3 */
    BTCLK     rPicoClock;                     /*Offset 0x10C*/ /* Offset 0xF4 */
    BTPHASE   rPicoPhase;                     /*Offset 0x110*/ /* Offset 0xF8 */
    U8        rPicoClkEn          : 1;        /*Offset 0x112*/ /* Offset 0xFA */
    U8        rPicoClkUpdate      : 1;        /*Offset 0x112*/ /* Offset 0xFA */
    U8        _reserved_byte_FAh  : 6;        /*Offset 0x112*/ /* Offset 0xFA */
    U8        _reserved_byte_FBh;             /*Offset 0x113*/ /* Offset 0xFB */
    BTCLK     rPicoClockOffset;               /*Offset 0x114*/ /* Offset 0xFC */
    BTPHASE   rPicoPhaseOffset;               /*Offset 0x118*/ /* Offset 0x100 */
    U16       _reserved_word_102h;            /*Offset 0x11A*/ /* Offset 0x102 */
    BTCLK     rCurrClockOffset;               /*Offset 0x11C*/ /* Offset 0x104 */
    U16       rCurrPhaseOffset;               /*Offset 0x120*/ /* Offset 0x108 */
    U16       _reserved_word_10Ah;            /*Offset 0x122*/ /* Offset 0x10A */

}CLK_CTL_STRU, * CLK_CTL_STRU_PTR;


typedef struct
{
    U8  rHopSeqSel;                            /* Offset 0x19C */  /* Offset 0x168 */
    U8  _reserved_byte_169h;                   /* Offset 0x19D */  /* Offset 0x169 */
    U8  rx_channel;                            /* Offset 0x19E */  /* Offset 0x16A */
    U8  tx_channel;                            /* Offset 0x19F */  /* Offset 0x16B */

}HOP_CTL_STRU, * HOP_CTL_STRU_PTR;

typedef struct
{
    U8  rNparam;                               /* Offset 0x16C */
    U8  rKoffset;                              /* Offset 0x16D */
    U8  rKnudge;                               /* Offset 0x16E */
    U8  _reserved_byte_16Fh;                   /* Offset 0x16F */

}INQUIRY_CTL_STRU, * INQUIRY_CTL_STRU_PTR;

typedef struct
{
    U8  rBt3AcCode2En       : 1;                /* Offset 0x170 */
    U8  _reserved_bits_170h : 7;                /* Offset 0x170 */
    U8  rNparam             : 5;                /* Offset 0x171 */
    U8  _reserved_bits_171h : 3;                /* Offset 0x171 */
    U8  rInterlaceOffset    : 5;                /* Offset 0x172 */
    U8  _reserved_bits_172h : 3;                /* Offset 0x172 */
    U8  _reserved_byte_173h;                    /* Offset 0x173 */

}INQUIRY_SCAN_CTL_STRU, * INQUIRY_SCAN_CTL_STRU_PTR;

typedef struct
{
    U8  rKoffset             : 5;               /* Offset 0x174 */
    U8  _reserved_bits_174h  : 3;               /* Offset 0x174 */
    U8  rKnudge              : 5;               /* Offset 0x175 */
    U8  _reserved_bits_175h  : 3;               /* Offset 0x175 */
    U8  rMasterPageRspNparam : 5;               /* Offset 0x176 */
    U8  _reserved_bits_176h  : 3;               /* Offset 0x176 */
    U8  _reserved_byte_177h;                    /* Offset 0x177 */

}PAGE_CTL_STRU, * PAGE_CTL_STRU_PTR;

typedef struct
{
    U8    _reserved_byte_178h;                 /* Offset 0x178 */
    U8    rSlavePageRspNparam;                 /* Offset 0x179 */
    U8    rInterlacedPageScan;                 /* Offset 0x17A */
    U8    _reserved_byte_17Ah;                 /* Offset 0x17B */

}PAGE_SCAN_CTL_STRU, * PAGE_SCAN_CTL_STRU_PTR;


typedef struct                                /* Offset 0x32C~ 0x34F */
{
    U32 rEn : 4;                              /* Offset 0x32C */
    U32 _reserved_byte_500h : 28;             /* Offset 0x32C */ /* Offset 0x500 */
    LE_ADDR_REG_STRU rAddr[4];                /* Offset 0x330 */ /* Offset 0x504 */

}WLCTL_STRU, * WLCTL_STRU_PTR;

typedef struct
{
    U32 rDiamEn : 4;
    U32 _reserved_byte_600h : 28;             /* Offset 0x600 */
    LE_ADDR_REG_STRU rAddr[4];                /* Offset 0x604 */

}DIACTL_STRU, * DIACTL_STRU_PTR;

typedef struct
{
    LE_ADDR_REG_STRU rAddr[4];                /* Offset 0x700 */

}RACTL_STRU, * RACTL_STRU_PTR;

typedef struct
{
    BTCLK    rTxClk;                         /* Offset 0x518 */     /* Offset 0x800 */
    BTPHASE  rTxPhs;                         /* Offset 0x51C */     /* Offset 0x804 */
    U8       rTxClkEn;                       /* Offset 0x51E */     /* Offset 0x806 */
    U8       rTxClkUdpt;                     /* Offset 0x51F */     /* Offset 0x807 */
    BTCLK    rTxClkOffset;                   /* Offset 0x520 */     /* Offset 0x808 */
    BTPHASE  rTxPhsOffset;                   /* Offset 0x524 */     /* Offset 0x80C */
    U16      _reserved_word_526h;            /* Offset 0x526 */     /* Offset 0x80E */
    BTCLK    rTxAncClk;                      /* Offset 0x528 */     /* Offset 0x810 */
    U16      rTxIntrvl;                      /* Offset 0x52C */     /* Offset 0x814 */
    U16      _reserved_word_52Eh;            /* Offset 0x52E */     /* Offset 0x816 */
    U32      rTxIntrTmr           : 21;      /* Offset 0x530 */     /* Offset 0x818 */
    U32      _reserved_bits_530h  : 11;      /* Offset 0x530 */     /* Offset 0x81A */
    U32      rTxFlushTmr          : 21;      /* Offset 0x534 */     /* Offset 0x81C */
    U32      _reserved_bits_534h  : 11;      /* Offset 0x534 */     /* Offset 0x81E */

    BTCLK    rRxClk;                         /* Offset 0x538 */     /* Offset 0x820 */
    BTPHASE  rRxPhs;                         /* Offset 0x53C */     /* Offset 0x824 */
    U8       rRxClkEn;                       /* Offset 0x53E */     /* Offset 0x826 */
    U8       rRxClkUdpt;                     /* Offset 0x53F */     /* Offset 0x827 */
    BTCLK    rRxClkOffset;                   /* Offset 0x540 */     /* Offset 0x828 */
    BTPHASE  rRxPhsOffset;                   /* Offset 0x544 */     /* Offset 0x82C */
    U16      _reserved_word_546h;            /* Offset 0x546 */     /* Offset 0x82E */
    BTCLK    rRxAncClk;                      /* Offset 0x548 */     /* Offset 0x830 */
    U16      rRxIntrvl;                      /* Offset 0x54C */     /* Offset 0x834 */
    U16      _reserved_word_54Eh;            /* Offset 0x54E */     /* Offset 0x836 */
    U32      rRxIntrTmr           : 21;      /* Offset 0x550 */     /* Offset 0x838 */
    U32      _reserved_bits_550h  : 11;      /* Offset 0x550 */     /* Offset 0x83A */
    U32      rRxAppndTmr          : 21;      /* Offset 0x554 */     /* Offset 0x83C */
    U32      _reserved_bits_554h  : 11;      /* Offset 0x554 */     /* Offset 0x83E */

    U32      rRxPthAvlbl          : 1;       /* Offset 0x558 */     /* Offset 0x844 */
    U32      rTxPthAvlbl          : 1;       /* Offset 0x558 */     /* Offset 0x844 */
    U32      _reserved_bits_844h  : 30;      /* Offset 0x558 */     /* Offset 0x844 */

    U32      rTxEn                : 1;       /* Offset 0x55C */      /* Offset 0x848 */
    U32      rTxSoftRst           : 1;       /* Offset 0x55C */      /* Offset 0x848 */
    U32      _reserved_bits_848h  : 6;       /* Offset 0x55C */      /* Offset 0x848 */
    U32      rTxAirMode           : 2;       /* Offset 0x55D */      /* Offset 0x849 */
    U32      _reserved_bits_849h  : 6;       /* Offset 0x55D */      /* Offset 0x849 */
    U32      rTxSrcAddrSelNxtSw   : 1;       /* Offset 0x55E */      /* Offset 0x84A */
    U32      _reserved_bits_84Ah  : 7;       /* Offset 0x55E */      /* Offset 0x84A */
    U32      rCommonClkEn         : 1;       /* Offset 0x55F */      /* Offset 0x84B */
    U32      _reserved_bits_84Bh  : 7;       /* Offset 0x55F */      /* Offset 0x84B */

    U32      rTxBufRdy            : 1;       /* Offset 0x560 */      /* Offset 0x84C */
    U32      _reserved_bits_84Ch  : 31;      /* Offset 0x560 */      /* Offset 0x84C */
    U32      rTxSrcAddr0;                    /* Offset 0x564 */      /* Offset 0x850 */
    U32      rTxSrcAddr1;                    /* Offset 0x568 */      /* Offset 0x854 */
    U32      rTxDataLen           : 10;      /* Offset 0x56C */      /* Offset 0x85C */
    U32      _reserved_bits_85Ch  : 22;      /* Offset 0x56C */      /* Offset 0x85C */

    U32      rRxEn                : 1;       /* Offset 0x570 */      /* Offset 0x860 */
    U32      rRxSoftRst           : 1;       /* Offset 0x570 */      /* Offset 0x860 */
    U32      _reserved_bits_860h  : 6;       /* Offset 0x570 */      /* Offset 0x860 */
    U32      rRxAirMode           : 2;       /* Offset 0x571 */      /* Offset 0x861 */
    U32      _reserved_bits_861h  : 6;       /* Offset 0x571 */      /* Offset 0x861 */
    U32      rRxDstAddrSelCurrSw  : 1;       /* Offset 0x572 */      /* Offset 0x862 */
    U32      _reserved_bits_862h  : 7;       /* Offset 0x572 */      /* Offset 0x862 */
    U32      rRxOk                : 1;       /* Offset 0x573 */      /* Offset 0x863 */
    U32      rRxOkClr             : 1;       /* Offset 0x573 */      /* Offset 0x863 */
    U32      _reserved_bits_863h  : 6;       /* Offset 0x573 */      /* Offset 0x863 */


    U32       rRxBufRdy           : 1;       /* Offset 0x574 */       /* Offset 0x864 */
    U32       _reserved_bits_864h : 7;       /* Offset 0x574 */       /* Offset 0x864 */
    U32       rRxAppendMuteEn     : 1;       /* Offset 0x575 */       /* Offset 0x865 */
    U32       _reserved_bits_865h : 23;      /* Offset 0x575 */       /* Offset 0x865 */

    U32      rRxDstAddr0;                    /* Offset 0x578 */       /* Offset 0x868 */
    U32      rRxDstAddr1;                    /* Offset 0x57C */       /* Offset 0x86C */
    U32      rRxDstOffset         : 16;      /* Offset 0x580 */       /* Offset 0x870 */
    U32      _reserved_word_872h  : 16;      /* Offset 0x582 */       /* Offset 0x872 */

    U32      rRxDataLen           : 10;      /* Offset 0x584 */      /* Offset 0x874 */
    U32      _reserved_bits_874h  : 22;      /* Offset 0x584 */       /* Offset 0x874 */

    U32      rRxCodecEn           : 1;       /* Offset 0x588 */      /* Offset 0x878 */
    U32      rTxCodecEn           : 1;       /* Offset 0x588 */       /* Offset 0x878 */
    U32      _reserved_bits_878h  : 2;       /* Offset 0x588 */       /* Offset 0x878 */
    U32      rRxCodecSoftRst      : 1;       /* Offset 0x588 */       /* Offset 0x878 */
    U32      rTxCodecSoftRst      : 1;       /* Offset 0x588 */       /* Offset 0x878 */
    U32      rRxDstAddrSoftRst    : 1;       /* Offset 0x588 */       /* Offset 0x878 */
    U32      rTxSrcAddrSoftRst    : 1;       /* Offset 0x588 */       /* Offset 0x878 */
    U32      rRxAppendStart       : 1;       /* Offset 0x589 */       /* Offset 0x879 */
    U32      rTxFlushStart        : 1;       /* Offset 0x589 */      /* Offset 0x879 */
    U32      rRxHasRxClr          : 1;       /* Offset 0x589 */      /* Offset 0x879 */
    U32      rTxHasTxClr          : 1;       /* Offset 0x589 */      /* Offset 0x879 */
    U32      _reserved_bits_879h  : 20;      /* Offset 0x589 */      /* Offset 0x879 */

    U32      rTxCodecSave         : 1;       /* Offset 0x58C */      /* Offset 0x87C */
    U32      rTxCodecStore        : 1;       /* Offset 0x58C */      /* Offset 0x87C */
    U32      rTxCodecSaveBusy     : 1;       /* Offset 0x58C */      /* Offset 0x87C */
    U32      rTxCodecStoreBusy    : 1;       /* Offset 0x58C */      /* Offset 0x87C */
    U32      _reserved_bits_87Ch  : 4;       /* Offset 0x58C */      /* Offset 0x87C */
    U32      rRxCodecSave         : 1;       /* Offset 0x58D */      /* Offset 0x87D */
    U32      rRxCodecStore        : 1;       /* Offset 0x58D */      /* Offset 0x87D */
    U32      rRxCodecSaveBusy     : 1;       /* Offset 0x58D */      /* Offset 0x87D */
    U32      rRxCodecStoreBusy    : 1;       /* Offset 0x58D */      /* Offset 0x87D */
    U32      _reserved_bits_87Dh  : 4;       /* Offset 0x58D */      /* Offset 0x87D */
    U32      rTxBusy              : 1;       /* Offset 0x58E */      /* Offset 0x87E */
    U32      _reserved_bits_87Eh  : 7;       /* Offset 0x58E */      /* Offset 0x87E */
    U32      rRxBusy              : 1;       /* Offset 0x58F */      /* Offset 0x87F */
    U32      _reserved_bits_87Fh  : 7;       /* Offset 0x58F */      /* Offset 0x87F */
    U8       rRxuLawMutePat;                 /* Offset 0x590 */      /* Offset 0x880 */
    U8       rRxALawMutePat;                 /* Offset 0x591 */      /* Offset 0x881 */
    U8       rRxCvsdMutePat;                 /* Offset 0x592 */      /* Offset 0x882 */
    U8       rRxTransMutePat;                /* Offset 0x593 */      /* Offset 0x883 */


}AUDIO_CTL_STRU, * AUDIO_CTL_STRU_PTR;

typedef union union_trx_timing_dbg
{
    struct TRX_TIMING_DBG_s
    {
        U32 cs:4;//bit 3~0 : cs[3:0]
        U32 BsRxOn:1;//bit 4:  bs_rxon
        U32 BsTxOn:1;//bit 5:  bs_txon
        U32 AcSyncP2M:1;//bit 6:  ac_sync_p_2m
        U32 TRxStart:1;//bit 7:  trx_start
        U32 DmaTxOrRxBusy:1;//bit 8:  dma_tx_busy | dma_rx_busy
        U32 AufwdRxBusy:1;//bit 9:  aufwd_rx_busy
        U32 AufwdTxBusy:1;//bit 10: aufwd_tx_busy
        U32 TxOrRxSbndP:1;//bit 11: tx_sbnd_p | rx_sbnd_p
        U32 CcmRxBusy:1;//bit 12: ccm_rx_busy
        U32 CcmTxBusy:1;//bit 13: ccm_tx_busy
        U32 SrchOn:1;//bit 14: srch_on
        U32 TickT0:1;//bit 15: tick_t0
        U32 ModRxOn:1;//bit 16: mod_rxon
        U32 ModTxOn:1;//bit 17: mod_txon
        U32 RfRxOn:1;//bit 18: rf_rxon
        U32 RfTxOn:1;//bit 19: rf_txon
        U32 PaOn:1;//bit 20: paon
        U32 SxOn:1;//bit 21: sxon
        U32 DmaRxOn:1;//bit 22: dma_rxon
        U32 DmaTxOn:1;//bit 23: dma_txon
        U32 AufwdTxOn:1;//bit 24: aufwd_txon
        U32 AufwdRxOn:1;//bit 25: aufwd_rxon
        U32 CcmOn:1;//bit 26: ccm_on
        U32 BleMode:1;//bit 27: ble_mode
        U32 Trx:1;//bit 28:  trx
        U32 CcmUse:1;//bit 29:  ccm_use
        U32 rsvd0:1;//bit 30: srch_on
        U32 rsvd1:1;//bit 31: tick_t0
    } field;
    U32 reg;
} TRX_TIMING_DBG_UNION, * TRX_TIMING_DBG_UNION_PTR;

typedef struct
{
    U32 hw_sts_dis:1;                               /* Offset 0x204 */
    U32 sts_vld_chk_en:1;
    U32 start_addr_sel:1;
	U32 rxble_brk_dis:1;
    U32 rssicnt_clr_dis:1;
	U32 unit_2m:2;
    U32 rsvd:1;
	U32 start_sel:3;
    U32 rsvd2:21;
    U8 per_ch_duration;                             /* Offset 0x208 */
    U8 per_ch_cal_time;
    U32 per_ch_2w_delay:6;
    U32 rsvd3:10;
    U32 ch_num:6;                                   /* Offset 0x20C */
    U32 rsvd4:2;
    U32 loop_count:5;
    U32 rsvd5:3;
    U32 per_ch_2w_num:2;
    U32 rsvd6:6;
    U32 start_2w_num:4;
    U32 end_2w_num:4;
    U32 sts1_buf_valid:1;                           /* Offset 0x210 */
    U32 sts2_buf_valid:1;
    U32 rsvd7:30;
    U32 trig_phase:12;                              /* Offset 0x214 */
    U32 rsvd8:4;
    U16 trig_ck;
    U32 rssi_sts1_addr;                             /* Offset 0x218 */
    U32 rssi_sts2_addr;                             /* Offset 0x21C */
    U16 valid_status;                               /* Offset 0x220 */
    U16 state;
    U32 start_trig;                                 /* Offset 0x224 */
    U32 cmd_start_addr1;                            /* Offset 0x228 */
    U32 cmd_start_addr2;                            /* Offset 0x22C */

} LPS_CTL_STRU, * LPS_CTL_STRU_PTR;

typedef struct
{
    U32 aes_key_ptr;                                /* Offset 0x230 */
    U32 aes_pdu_ptr;                                /* Offset 0x234 */
    U32 aes_out_ptr;                                /* Offset 0x238 */
	U32 rsvd0;										/* Offset 0x23C */
    U32 rsvd1:1;									/* Offset 0x240 */
	U32 sfr_data_clk_free:1;
	U32 sfr_mic_clk_free:1;
	U32 sfr_aes_clk_free:1;
    U32 rsvd2:28;
    U32 aes_start;                                  /* Offset 0x244 */
	U32 rsvd3;						                /* Offset 0x248 */
    U32 ccm_aes_busy:1;                             /* Offset 0x24C */
    U32 aes_busy:1;
    U32 rsvd4:30;

} AES_CTL_STRU, * AES_CTL_STRU_PTR;

typedef struct stru_wht_ctl_br
{
    U32 rgf_wh_list_vld_l;                       /* Offset 0x2E0 */
    U32 rgf_wh_list_vld_h;                       /* Offset 0x2E4 */
    U32 hw_wh_comp_done         : 1;             /* Offset 0x2E8 */
    U32 hw_wh_match             : 1;
    U32 hw_wh_comp_grant        : 1;
    U32 hw_wh_comp_prep_done    : 1;
    U32 hw_wh_match_idx         : 6;
    U32 _reserved_dword_2E8h    : 21;
    U32 rgf_wh_enable           : 1;
    U32 wh_mem_ptr;                              /* Offset 0x2EC */

} WHT_CTL_BR_STRU, * WHT_CTL_BR_STRU_PTR;


typedef struct stru_reso_compare_result
{
    U16 hw_reso_lkey_match      : 1;             /* Offset 0x2F4 */
    U16 hw_reso_pkey_match      : 1;
    U16 hw_reso_pida_match      : 1;
    U16 _reserved_bits_2F4h     : 1;
    U16 hw_reso_match_idx       : 6;
    U16 hw_reso_lkey_zero       : 1;
    U16 hw_reso_pkey_zero       : 1;
    U16 hw_reso_comp_done       : 1;
    U16 hw_reso_match           : 1;
    U16 hw_reso_comp_grant      : 1;
    U16 hw_reso_comp_prep_done  : 1;

}s_RESO_COMP_RESULT, *s_RESO_COMP_RESULT_PTR;

typedef union union_reso_compare_result
{
    s_RESO_COMP_RESULT field;
    U16 value;

}RESO_COMP_RESULT, *RESO_COMP_RESULT_PTR;


typedef struct stru_reso_ctl_br
{
    U32 reso_list_vld;                           /* Offset 0x2F0 */
    RESO_COMP_RESULT hw_reso_comp_result;        /* Offset 0x2F4 */        
    U16 _reserved_byte_2F6h     : 12;            /* Offset 0x2F6 */
    U16 rgf_lkey_enable         : 1;
    U16 sfr_tx_local_rpa_replace_en : 1;
    U16 rgf_write_resoAdva_enable : 1;
    U16 rgf_reso_enable         : 1;
    U32 rpa_mem_offset;                          /* Offset 0x2F8 */

} RESO_CTL_BR_STRU, * RESO_CTL_BR_STRU_PTR; //backup and restore

typedef struct stru_txfhs_ctl
{
    U32 rsvd0                   : 2;              /* Offset 0x730 */
    U32 sfr_fhs_nclk_sw         : 26;
    U32 rsvd1                   : 3;
    U32 sfr_fhs_nclk_sw_en      : 1;

} BT3_TXFHS_MAN_STRU, * BT3_TXFHS_MAN_STRU_PTR;

typedef struct stru_reso_ctl
{
    U32 _reserved_bit0_914h         : 1;                /* Offset 0x914 */
    U32 rgf_no_init_tagta           : 1;
    U32 _reserved_bit2_914h         : 1;
    U32 _reserved_bit3_914h         : 1;
    U32 hw_comp_done                : 1;
    U32 hw_comp_match               : 1;
    U32 hw_comp_grant               : 1;
    U32 _reserved_dword_914h        : 19;
    U32 rgf_hw_comp_abort_en        : 1;
    U32 rgf_prvc_sw_rst             : 1;
    U32 rgf_prvc_sw_rst_cr          : 1;
    U32 privacy_manual_clk          : 1;
    U32 ble_low_power_off           : 1;
    U32 rgf_match_clr               : 1;
    U32 rgf_reso_adva_l;                                /* Offset 0x918 */
    U16 rgf_reso_adva_h;                                /* Offset 0x91C */
    U16 rgf_reso_init_tagta_h;                          /* Offset 0x91C */
    U32 rgf_reso_init_tagta_l;                          /* Offset 0x920 */
    U32 rgf_reso_swdef;                                 /* Offset 0x924 */
    U32 fw_reso_lkey_match          : 1;                /* Offset 0x928 */
    U32 fw_reso_pkey_match          : 1;
    U32 fw_reso_pida_match          : 1;
    U32 fw_reso_swdef_match         : 1;
    U32 fw_reso_match_idx           : 6;
    U32 fw_reso_lkey_zero           : 1;
    U32 fw_reso_pkey_zero           : 1;
    U32 fw_reso_comp_done           : 1;
    U32 fw_reso_match               : 1;
    U32 fw_reso_comp_grant          : 1;
    U32 fw_reso_comp_prep_done      : 1;
    U32 _reserved_byte_92Ah         : 9;                /* Offset 0x92A */
    U32 rgf_reso_init_tagta_typeid  : 1;
    U32 rgf_reso_adva_typeid        : 1;
    U32 rgf_reso_fw_comp_mask       : 1;
    U32 rgf_fw_match_clr            : 1;
    U32 rgf_fw_comp_lp_addr         : 2;
    U32 rgf_reso_comp_trig          : 1;
    U32 hw_reso_lkey_last_match_idx : 6;                /* Offset 0x92C */
    U32 _reserved_bits_92Ch         : 1;
    U32 hw_reso_lkey_last_match     : 1;
    U32 hw_reso_pkey_last_match_idx : 6;                /* Offset 0x92D */
    U32 _reserved_bits_92Dh         : 1;
    U32 hw_reso_pkey_last_match     : 1;
    U32 hw_reso_pida_last_match_idx : 6;                /* Offset 0x92E */
    U32 _reserved_bits_92Eh         : 1;
    U32 hw_reso_pida_last_match     : 1;
    U8  _reserved_byte_92Fh;                            /* Offset 0x92F */
    U32 fw_reso_lkey_last_match_idx : 6;                /* Offset 0x930 */
    U32 _reserved_bits_930h         : 1;
    U32 fw_reso_lkey_last_match     : 1;
    U32 fw_reso_pkey_last_match_idx : 6;                /* Offset 0x931 */
    U32 _reserved_bits_931h         : 1;
    U32 fw_reso_pkey_last_match     : 1;
    U32 fw_reso_pida_last_match_idx : 6;                /* Offset 0x932 */
    U32 _reserved_bits_932h         : 1;
    U32 fw_reso_pida_last_match     : 1;
    U8  _reserved_byte_933h;                            /* Offset 0x933 */
    U8  rgf_addrid[6];                                  /* Offset 0x934 */
    U32 rgf_typeid                  : 1;
    U32 _reserved_byte_93Ah         : 15;               /* Offset 0x93A */
    U32 reso_srch_done              : 1;                /* Offset 0x93C */
    U32 reso_srch_grant             : 1;
    U32 reso_srch_match_idx         : 6;
    U32 reso_srch_match             : 1;
    U32 _reserved_dword_93Ch        : 21;
    U32 rgf_srch_vld_clr            : 1;
    U32 rgf_srch_en                 : 1;
    U32 reso_srch_vld;                                  /* Offset 0x940 */
    U32 rgf_rpa_random              : 24;               /* Offset 0x944 */
    U32 rgf_key_idx                 : 6;                /* Offset 0x947 */
    U32 _reserved_bits_947h         : 2;
    U32 gen_rpa_done                : 1;                /* Offset 0x948 */
    U32 gen_rpa_grant               : 1;
    U32 rpa_lkey_zero               : 1;
    U32 rpa_pkey_zero               : 1;
    U32 gen_rpa_prep_done           : 1;
    U32 _reserved_byte_948h         : 22;
    U32 rgf_rpa_clr                 : 1;
    U32 rgf_rpa_done_clr            : 1;
    U32 rgf_lp_rpa                  : 2;
    U32 rgf_rpa_manual_trig         : 1;
    U32 l_rpa_l;                                        /* Offset 0x94C */
    U16 l_rpa_h;                                        /* Offset 0x950 */
    U16 p_rpa_h;                                        /* Offset 0x952 */
    U32 p_rpa_l;                                        /* Offset 0x954 */
    U32 hw_comp_abort_done          : 1;                /* Offset 0x958 */
    U32 hw_comp_abort_status        : 3;
    U32 _reserved_dword_958h        : 26;
    U32 rgf_hw_comp_abort_clr       : 1;
    U32 rgf_hw_comp_do_abort        : 1;

} RESO_CTL_STRU, * RESO_CTL_STRU_PTR;

typedef struct stru_wht_ctl
{
    U8 rgf_wh_adav[6];                                 /* Offset 0x95C */
    U32 rgf_wh_typeid               : 1;                /* Offset 0x962 */
    U32 rgf_wh_sw_def               : 15;
    U32 fw_wh_srch_done             : 1;                /* Offset 0x964 */
    U32 fw_wh_done                  : 1;
    U32 fw_wh_match                 : 1;
    U32 wh_srch_match               : 1;
    U32 fw_wh_idx                   : 6;
    U32 _reserved_btis_965h         : 1;
    U32 fw_wh_swdef_match           : 1;
    U32 fw_wh_adva_match            : 1;
    U32 fw_wh_comp_grant            : 1;
    U32 fw_wh_srch_grant            : 1;
    U32 fw_wh_comp_prep_done        : 1;
    U32 fw_wh_srch_prep_done        : 1;                /* Offset 0x966 */
    U32 wh_srch_match_idx           : 6;
    U32 _reserved_byte_966h         : 3;
    U32 rgf_wh_fw_comp_mask         : 1;
    U32 rgf_srch_wh_clr             : 1;
    U32 rgf_fw_wh_clr               : 1;
    U32 rgf_wh_comp_srch            : 2;
    U32 rgf_wh_manual_trig          : 1;
    U32 fw_wh_srch_vld_l;                               /* Offset 0x968 */
    U32 fw_wh_srch_vld_h;                               /* Offset 0x96C */

} WHT_CTL_STRU, * WHT_CTL_STRU_PTR;

#if 0
typedef struct s_bb_packet_cmd_registers {

    U16  rBt3RxCmdRslt;                        /* Offset 0x244 */
    U16  rBt3TxCmdRslt;                        /* Offset 0x246 */
    U16  rBleRxCmdRslt;                        /* Offset 0x248 */
    U16  rBleTxCmdRslt;                        /* Offset 0x24A */

    U16  rBt3RxPktType;                        /* Offset 0x24C */
    U16  _reserved_word_24Eh;                  /* Offset 0x24E */
    U16  rBleRxPktType;                        /* Offset 0x250 */
    U16  _reserved_word_252h;                  /* Offset 0x252 */

    U16  rBt3RxPktHdr;                         /* Offset 0x254 */
    U16  rBt3RxPldHdr;                         /* Offset 0x256 */
    U32  _reserved_dword_258h;                 /* Offset 0x258 */
    U32  _reserved_dword_25Ch;                 /* Offset 0x25C */
    U16  rBleRxPduHdr;                         /* Offset 0x260 */
    U16  _reserved_word_262h;                  /* Offset 0x262 */

    BD_ADDR_STRU  rBleRcvdTxAddr;              /* Offset 0x264 */
    U8   rBleRcvdTxAddrType;                   /* Offset 0x26A */
    U8   _reserved_word_26Bh;                  /* Offset 0x26B */
    BD_ADDR_STRU  rBleRcvdRxAddr;              /* Offset 0x26C */
    U8   rBleRcvdRxAddrType;                   /* Offset 0x272 */
    U8   _reserved_word_273h;                  /* Offset 0x273 */


} t_bb_packet_cmd_registers;
#endif

typedef struct s_bb_rx_buffer_registers {
    U16  rOffset;                            /* Offset 0x1B8 */  /* Offset 0x2AC */
    U16  _reserved_dword_2AEh;               /* Offset 0x1BA */  /* Offset 0x2AE */
    U16  rBuf0Len;                           /* Offset 0x1BC */  /* Offset 0x2B0 */
    U16  _reserved_word_2B2h;                /* Offset 0x1BE */  /* Offset 0x2B2 */
    U32  rBuf0Addr;                          /* Offset 0x1C0 */  /* Offset 0x2B4 */
    U16  rBuf1Len;                           /* Offset 0x1C4 */  /* Offset 0x2B8 */
    U16  _reserved_word_2BAh;                /* Offset 0x1C6 */  /* Offset 0x2BA */
    U32  rBuf1Addr;                          /* Offset 0x1C8 */  /* Offset 0x2BC */
    U16  rBuf2Len;                           /* Offset 0x1CC */  /* Offset 0x2C0 */
    U16  _reserved_word_2C2h;                /* Offset 0x1CE */  /* Offset 0x2C2 */
    U32  rBuf2Addr;                          /* Offset 0x1D0 */  /* Offset 0x2C4 */
    U8   rStatus;                            /* Offset 0x1D4 */  /* Offset 0x2C8 */
    U8   _reserved_word_2C9h;                /* Offset 0x1D5 */  /* Offset 0x2C9 */
    U8   rValidity;                          /* Offset 0x1D6 */  /* Offset 0x2CA */
    U8   _reserved_word_2CBh;                /* Offset 0x1D7 */  /* Offset 0x2CB */
    U8   rPolicy;                            /* Offset 0x1D8 */  /* Offset 0x2CC */
    U8   _reserved_byte_2CDh;                /* Offset 0x1D9 */  /* Offset 0x2CD */
    U16  _reserved_word_2CEh;                /* Offset 0x1DA */  /* Offset 0x2CE */
} t_RxBuf;


typedef union u_trx_manual_enable_control {            /* Offset 0x700 */

    struct s_trx_manual_enable_control {

        U32 rsvd0           : 7;     //- bit 0-6
        U32 ChannelEn       : 1;     //- bit 7
        U32 rsvd1           : 1;     //- bit 8
        U32 PktEndSwEn      : 1;     //- bit 9
        U32 ModRxOnSwEn     : 1;     //- bit 10
        U32 ModTxOnSwEn     : 1;     //- bit 11
        U32 PaOnSwEn        : 1;     //- bit 12
        U32 RfRxOnSwEn      : 1;     //- bit 13
        U32 RfTxOnSwEn      : 1;     //- bit 14
        U32 SxOnSwEn        : 1;     //- bit 15
        U32 HopDoneSwEn     : 1;     //- bit 16
        U32 BgLdoSwEn       : 1;     //- bit 17
        U32 BbPllSwEn       : 1;     //- bit 18
        U32 BbPllRdySwEn    : 1;     //- bit 19
        U32 BbAdcSwEn       : 1;     //- bit 20
        U32 BbDacSwEn       : 1;     //- bit 21
        U32 BbPllCkSelSwEn  : 1;     //- bit 22
        U32 rsvd2           : 1;     //- bit 23
        U32 TrxIsRxSwEn     : 1;     //- bit 24
        U32 BleModeSwEn     : 1;     //- bit 25

    }field;
    U32 reg;

}t_rTrxManualEnCtl;


typedef union u_trx_manual_ctl {                         /* Offset 0x705 */ /* Offset 0x45D */

    struct s_trx_manual_ctl {
        // U8  rManualChannelEn     : 1;
        // U8  rEnable              : 1;
        U8  rsvd                 : 1;
        U8  rPktEnd              : 1;
        U8  rModRxon             : 1;
        U8  rModTxon             : 1;
        U8  rPaon                : 1;
        U8  rRfRxon              : 1;
        U8  rRfTxon              : 1;
        U8  rSxon                : 1;
    }field;
    U8 reg;

}t_rTrxManualCtl;



typedef struct s_bt3_misc_ctl {                  /* Offset 0x1A8 */  /* Offset 0x3E8 */
    U32  rWhiteningNormalEn   : 1;
    U32  rAcCode2ForceEn      : 1;
    U32  Rsvd0                : 6;
    U32  rRxHecForceOk        : 1;
    U32  rTxHecForceErr       : 1;
    U32  rTxCrcForceErr       : 1;
    U32  rAmEv3Hv3_sel        : 1;
    U32  rForceEscoAddrMatch  : 1;
    U32  rForceAclAddrMatch   : 1;
    U32  Rsvd1                : 18;
} tBt3MiscCtl;


typedef union u_bt3_rx_ctl {       /* Offset 0x1B0 */

    struct s_bt3_rx_ctl{         /* Offset 0x1B0 */  /* Offset 0x28C */
    U32 rAccCodeAbort : 8;
    U32 rLtAddrAbort  : 8;
    U32 rLengthAbort  : 8;
    U32 rLlidAbort    : 8;
    }field;

    U32 reg;

}tBt3RxCtl;


#if 0
typedef struct s_bt3_rx_ctl{                      /* Offset 0x1B0 */  /* Offset 0x28C */
    U32 rAccCodeAbort : 8;
    U32 rLtAddrAbort  : 8;
    U32 rLengthAbort  : 8;
    U32 rLlidAbort    : 8;

} tBt3RxCtl;
#endif

typedef struct s_ble_misc_ctl {                    /* Offset 0x314 */   /* Offset 0x3EC */
    U32  rWhiteningNormalEn   : 1;
    U32  Rsvd0                : 7;
    U32  rTxCrcForceErr       : 1;
    U32  Rsvd1                : 23;
} tBleMiscCtl;


/* the configuration registers consisting of FHS parameters. */

typedef struct s_bb_fhs_config_registers {
    U8       rFhsEirBit;                       /* Offset 0x510 */
    U8       rFhsLtAddr;                       /* Offset 0x511 */
    U16      _reserved_word_512h;              /* Offset 0x512 */
    U8       rFhsSrMode;                       /* Offset 0x514 */
    U8       rFhsPsMode;                       /* Offset 0x515 */
    U8       _reserved_byte_516h;              /* Offset 0x516 */
    U8       rFhsRsvdField;                    /* Offset 0x517 */
} tFhsCfg;

typedef struct s_bb_fhs_parsing_registers {
    PARITY_STRU rFhsRemoteParity;                /* Offset 0x62C */
    U8     _reserved_byte_631h;                  /* Offset 0x631 */
    U16    _reserved_word_632h;                  /* Offset 0x632 */
    BD_ADDR_STRU  rFhsRemoteBdAddr;              /* Offset 0x634 */
    U16    _reserved_word_63Ah;                  /* Offset 0x63A */

    U8     rFhsRemoteEirBit;                     /* Offset 0x63C */
    U8     rFhsRemoteLtAddr;                     /* Offset 0x63D */
    U16    _reserved_word_63Eh;                  /* Offset 0x63E */
    U8     rFhsRemoteSrMode;                     /* Offset 0x640 */
    U8     rFhsRemotePsMode;                     /* Offset 0x641 */
    U16    _reserved_word_642h;                  /* Offset 0x642 */
    U32    rFhsRemoteCoD;                        /* Offset 0x644 */
    BTCLK  rFhsRemoteClk;                        /* Offset 0x648 */
} tRmtFhs;


typedef union u_tx_redundant_control {       /* Offset 0x180 */

    struct s_tx_redundant_control {

        U8 br_edr_bit_num;
        U8 br_edr_pattern_sel;
        U8 ble_bit_num;
        U8 ble_pattern_sel;
    }field;
    U32 reg;

}t_rTxRedundantCtl;



typedef struct s_rf_2wire_debug_control
{
    U32 rMasterEn        : 1;   //- bit 0
    U32 rCapNext         : 1;   //- bit 1
    U32 rsvd0            : 2;   //- bit 2-3
    U32 rSpiFsmBusy      : 1;   //- bit 4
    U32 rSpiSclState     : 2;   //- bit 5-6
    U32 rsvd1            : 1;   //- bit 7
    U32 rSpiSdaState     : 4;   //- bit 8-11
    U32 rsvd2            : 4;   //- bit 12-15
    U32 rAddrNakFlag     : 1;   //- bit 16
    U32 rDataNakFlag     : 1;   //- bit 17
    U32 rsvd3            : 6;   //- bit 18-23
    U32 rAddrNakFlagClr  : 1;   //- bit 24
    U32 rDataNakFlagClr  : 1;   //- bit 25
    U32 rsvd4            : 6;   //- bit 26-31
    U32 rRf2WireTopCs    : 4;   //- bit 0-3
    U32 rsvd5            : 28;  //- bit 4-31

}s_RF_2WIRE_DBG_CTL_STRU,*s_RF_2WIRE_DBG_CTL_STRU_PTR;


typedef union u_rf_2wire_debug_control {

    s_RF_2WIRE_DBG_CTL_STRU field;
    U32 reg[2];

}u_RF_2WIRE_DBG_CTL_STRU, *u_RF_2WIRE_DBG_CTL_STRU_PTR;

typedef struct stru_software_2wire_spi_command_information {
    U32 rCmdAddr    : 9;           /* Offset 0xA0 */
    U32 rsvd0       : 7;
    U32 rCmdDLen    : 3;           /* Offset 0xA2 */
    U32 rsvd1       : 5;
    U32 rCmdIsRd    : 1;           /* Offset 0xA3 */
    U32 rsvd2       : 7;

}SW_2WIRE_CMD_INFO_STRU, *SW_2WIRE_CMD_INFO_STRU_PTR;

typedef union union_software_2wire_spi_command_information {

    SW_2WIRE_CMD_INFO_STRU field;
    U32 reg;

}t_SW_2WIRE_CMD_INFO_STRU, *t_SW_2WIRE_CMD_INFO_STRU_PTR;


typedef struct stru_software_2wire_spi_control {

    U32 rSwStart;                  /* Offset 0x88 */

    t_SW_2WIRE_CMD_INFO_STRU rCmdInfo; /* Offset 0x8C */

    U32 rCmdWData[2];              /* Offset 0x90 - 0x97 */
    U32 rCmdRData[2];              /* Offset 0x98 - 0x9F */

}SW_2WIRE_SPI_CTL_STRU, *SW_2WIRE_SPI_CTL_STRU_PTR;


typedef struct stru_spi_frequency_misc_control1 {
    U32 rFreqAddr0ReplaceEn    : 1;  /* bit 0 */
    U32 rFreqAddr1ReplaceEn    : 1;  /* bit 1 */
    U32 rFreqAddr2ReplaceEn    : 1;  /* bit 2 */
    U32 rFreqAddr3ReplaceEn    : 1;  /* bit 3 */
    U32 rFreqAddr4ReplaceEn    : 1;  /* bit 4 */
    U32 rFreqAddr5ReplaceEn    : 1;  /* bit 5 */
    U32 rFreqAddr6ReplaceEn    : 1;  /* bit 6 */
    U32 rFreqAddr7ReplaceEn    : 1;  /* bit 7 */
    U32 rFreqNegifBpfReplaceEn : 1;  /* bit 8 */
    U32 rFreqSdmHwRstReplaceEn : 1;  /* bit 9 */
    U32 rFreqSxSdmReplaceEn    : 1;  /* bit 10 */
    U32 rFreqSxFcalReplaceEn   : 1;  /* bit 11 */
    U32 rFreqEdrReplaceEn      : 1;  /* bit 12 */
    U32 rFreqBbPllAdcReplaceEn : 1;  /* bit 13 */
    U32 rFreqBbPllHopReplaceEn : 1;  /* bit 14 */
    U32 rFreqTpmPathReplaceEn  : 1;  /* bit 15 */
    U32 rFreqBbPll16mReplaceEn : 1;  /* bit 16 */
    U32 rFreqCapCalReplaceEn   : 1;  /* bit 17 */
    U32 rFreqFcalTarReplaceEn  : 1;  /* bit 18 */
    U32 rFreqModeReplaceEn     : 1;  /* bit 19 */
    U32 rFreqDiv2ReplaceEn     : 1;  /* bit 20 */
    U32 rFreqNegifReplaceEn    : 1;  /* bit 21 */

    U32 rsvd : 10 ;

}s_2WIRE_SPI_FREQ_MISC_CTL1_STRU, *s_2WIRE_SPI_FREQ_MISC_CTL1_STRU_PTR;

typedef union union_2wire_spi_frequency_misc_control1 {

    s_2WIRE_SPI_FREQ_MISC_CTL1_STRU field;
    U32 reg;

}u_2WIRE_SPI_FREQ_MISC_CTL1_STRU, *u_2WIRE_SPI_FREQ_MISC_CTL1_STRU_PTR;




typedef struct stru_spi_frequency_misc_control2 {
    U32 rEdrPtrEn           : 1;   /* bit 0 */
    U32 rBlePtrEn           : 1;   /* bit 1 */
    U32 r2WireOnAutoCalDis  : 1;   /* bit 2 */
    U32 r2WireOffAutoCalDis : 1;   /* bit 3 */
    U32 rsvd0               : 4;   /* bit 4-7  */
    U32 rFcalOffsetVal      : 3;   /* bit 8-10 */

    U32 rsvd1               : 13;  /* bit 11-23 */
    U32 rNegIfInv           : 1;   /* bit 24 */
    U32 rsvd2               : 7;  /* bit 11-23 */


}s_2WIRE_SPI_FREQ_MISC_CTL2_STRU, *s_2WIRE_SPI_FREQ_MISC_CTL2_STRU_PTR;


typedef union union_2wire_spi_frequency_misc_control2 {

    s_2WIRE_SPI_FREQ_MISC_CTL2_STRU field;
    U32 reg;

}u_2WIRE_SPI_FREQ_MISC_CTL2_STRU, *u_2WIRE_SPI_FREQ_MISC_CTL2_STRU_PTR;


typedef struct s_rf_2wire_control {

    u_RF_2WIRE_DBG_CTL_STRU  rHwDbg;               /* Offset 0x80 - 0x87 */
    SW_2WIRE_SPI_CTL_STRU  rSwSpiCtl;              /* Offset 0x88 - 0x9F */

    U32 rGuardTime : 8;                            /* Offset 0xA0 */
    U32 _reserved_dword_A0h : 24;
    U32 rBusy   : 8;                               /* Offset 0xA4 */
    U32 rBusyHw : 8;
    U32 rSpi2SwRst : 8;
    U32 rSPiBurstShiftCtl: 8;

    u_2WIRE_SPI_FREQ_MISC_CTL1_STRU rFreqMiscCtl1; /* Offset 0xA8 */
    u_2WIRE_SPI_FREQ_MISC_CTL2_STRU rFreqMiscCtl2; /* Offset 0xAC */

    U32 rBleTxOnCmdPtr;                            /* Offset 0xB0 */
    U32 rBleTxOffCmdPtr;                           /* Offset 0xB4 */
    U32 rBleRxOnCmdPtr;                            /* Offset 0xB8 */
    U32 rBleRxffCmdPtr;                            /* Offset 0xBC */
    U32 rBrTxOnCmdPtr;                             /* Offset 0xC0 */
    U32 rBrTxOffCmdPtr;                            /* Offset 0xC4 */
    U32 rEdrTxOnCmdPtr;                            /* Offset 0xC8 */
    U32 rEdrTxOffCmdPtr;                           /* Offset 0xCC */
    U32 rBt3RxOnCmdPtr;                            /* Offset 0xD0 */
    U32 rBt3RxffCmdPtr;                            /* Offset 0xD4 */

    U32 _reserved_dword_D8h[2];                    /* Offset 0xD8-0xDF */

    U16 rInFracAddr[8];                            /* Offset 0xE0-0xEF */

    U32 rChParamMemPtr;                            /* Offset 0xF0 */

    U32 _reserved_dword_F4h[3];                    /* Offset 0xF4-0xFF */

}s_RF_2WIRE_CTL_STRU, *s_RF_2WIRE_CTL_STRU_PTR;



typedef struct stru_ble_hop_manual_misc_control0{

    U8  Rsvd1;              /* Offset 0x3E8 */
    U8  rFchLastUnMap;      /* Offset 0x3E9 */
    U8  rIgnoreChMap;       /* Offset 0x3EA */
    U8  rManualModeEn;      /* Offset 0x3EB */

 }BLE_HOP_MANUAL_MISC_CTL0, * BLE_HOP_MANUAL_MISC_CTL0_PTR;


typedef union union_ble_hop_manual_misc_control0 {

    BLE_HOP_MANUAL_MISC_CTL0 field;
    U32 reg;

}u_BLE_HOP_MANUAL_MISC_CTL0, *u_BLE_HOP_MANUAL_MISC_CTL0_PTR;

typedef struct stru_ble_hop_manual_control {

    u_BLE_HOP_MANUAL_MISC_CTL0 rMiscCtl0;

    U32 rFchChMap[2];       /* Offset 0x3EC~ 0x3F3*/
    U16 rFchEvtCnt;         /* Offset 0x3F4~ 0x3F5*/
    U8  rFchIsoSe;          /* Offset 0x3F6 */
    U8  rFchHop2En;         /* Offset 0x3F7 */
    U32 rFchAccessAddr;     /* Offset 0x3F8 ~ 0x3FB */
    U32 rFchLastSeInfo;     /* Offset 0x3FC ~ 0x3FF */
    U8  rHopOn;             /* Offset 0x400 */
    U8  rSwChannelEn;       /* Offset 0x401 */
    U8  rSwChannel;         /* Offset 0x402 */
    U8  Rsvd2;              /* Offset 0x403 */

 }BLE_HOP_MANUAL_CTL, * BLE_HOP_MANUAL_CTL_PTR;

typedef struct stru_ble_manual_hop_status {

    U8  rHopUnMapCh;        /* Offset 0x404 */
    U8  Rsvd3;              /* Offset 0x405 */
    U8  rDataHopCode;       /* Offset 0x406 */
    U8  rHopReady;          /* Offset 0x407 */
    U32 rHop2CurrSeInfo;    /* Offset 0x408 ~ 0x40B */

 } BLE_MANUAL_HOP_STATUS, *BLE_MANUAL_HOP_STATUS_PTR;


/* Hardware Baseband Registers
 * --
 * The data structure expresses the layout of the hardware baseband registers. */

typedef struct s_hardware_baseband_registers
{
    U32 _reserved_dword_0h[32];                   /* Offset 0x00 - 0x7F */
    s_RF_2WIRE_CTL_STRU rRf2WireCtl;              /* Offset 0x80 - 0xF7 */

    U32 bt_clk_en             : 1;                /* Offset 0x100 */ /* Offset 0xA40 */
    U32 _reserved_byte_100h   : 3;
    U32 sfr_lps_clk_free_en   : 1;
    U32 sfr_2w_clk_free_en    : 1;
    U32 sfr_rpa_clk_free_en   : 1;
    U32 _reserved_dword_100h  : 25;               /* Offset 0x100 */ /* Offset 0xA40 */

    CLK_CTL_STRU rClkCtl;                         /* Offset 0x104  ~ 0X123 */ /* Offset 0xEC  ~ 0X10B */

    MODEM_CTL_STRU rModemCtl;                     /* Offset 0x124  ~ 0x15F  */ /* Offset 0x80  ~ 0xBF  */


    U32 rBgldoSettleTime : 8;                    /* Offset 0x160 */
    U32 rBgldoOffDelay   : 6;
    U32 _reserved_byte_161h  : 2;                /* Offset 0x161 */
    U32 rPllClkSelSettleTime : 8;                /* Offset 0x162 */
    U32 rPllClkSelOffDelay   : 6;
    U32 _reserved_byte_163h : 2;

    U32 rPllSettleTime : 8;                      /* Offset 0x164 */
    U32 rPllOffDelay   : 6;
    U32 _reserved_byte_165h : 2;
    U32 rPllRdySettleTime : 8;                   /* Offset 0x166*/
    U32 rPllRdyOffDelay   : 6;
    U32 _reserved_byte_167h : 2;

    U32 rAdcSettleTime : 8;                      /* Offset 0x168 */
    U32 rAdcOffDelay   : 6;
    U32 _reserved_dword_169h : 2;
    U32 rDacSettleTime : 8;                      /* Offset 0x16A */
    U32 rDacOffDelay   : 6;
    U32 _reserved_dword_16Bh : 2;

    U32 rbb64mOnSettleTime : 8;                   /* Offset 0x16C */
    U32 rbb64mOffDly : 8;
    U32 rbb64mOnOffDis : 1;
    U32 _reserved_dword_16Eh : 15;

    U8 rBleSrchWinPreOnTime;                      /* Offset 0x170 */
    U8 rBt3SrchWinPreOnTime;
    U16 _reserved_word_172h;

    U8 rBleBsTxSettileTime;                       /* Offset 0x174 */
    U8 rBleBsTxSettileTime2M;
    U8 rBt3BsTxSettileTime;
    U8 _reserved_byte_177h;

    U32 rBtBusyOffDly : 8;                        /* Offset 0x178 */
    U32 _reserved_dword_178h : 24;

    U8 rNewRampDuration;                          /* Offset 0x17C */
    U8 rNewGfskOffDelay;
    U8 rNewModemOffDelay;
    U8 _reserved_byte_17Fh;

    t_rTxRedundantCtl rTxRedundantCtl;            /* Offset 0x180 */

    U32  rLttAddr;                                /* Offset 0x184 */ /* Offset 0x290 */

    LOCAL_DEVICE_CONFIG_STRU rLclDvcCfg;          /* Offset 0x188  ~ 0x19B  */ /* Offset 0x00  ~ 0x13  */

    HOP_CTL_STRU rHopCtl;                         /* Offset 0x19C ~ 0x19F */  /* Offset 0x168 ~ 0x16B */

    U32  rBt3CmdWord;                           /* Offset 0x1A0 */  /* Offset 0x238 */
    U8   rCmdTrig;                              /* Offset 0x1A4 */  /* Offset 0x234 */
    U8   rCmdAbort;                             /* Offset 0x1A5 */  /* Offset 0x235 */
    U8   rTsvrBusy;                             /* Offset 0x1A6 */  /* Offset 0x236 */
    U8   rCmdRst;                               /* Offset 0x1A7 */  /* Offset 0x237 */

    tBt3MiscCtl  rBt3MiscCtl;                   /* Offset 0x1A8 */  /* Offset 0x3E8 */

    U32 rBtDbgSel            : 2;               /* Offset 0x1AC */  /* Offset 0xA00 */
    U32 rBt3HopisScanAll     : 1;               /* Offset 0x1AC */  /* Offset 0xA00 */
    U32 _reserved_bits_A00h  : 5;               /* Offset 0x1AC */  /* Offset 0xA00 */

    U32 rBt3AutxTypeChkEn    : 1;               /* Offset 0x1AD */  /* Offset 0xA01 */
    U32 rBt3TxPlhConv        : 1;               /* Offset 0x1AD */  /* Offset 0xA01 */
    U32 rBt3AutxAutoApndEn   : 1;               /* Offset 0x1AD */  /* Offset 0xA01 */
    U32 rBt3AurxAutoApndEn   : 1;               /* Offset 0x1AD */  /* Offset 0xA01 */
    U32 _reserved_bits_A01h  : 4;               /* Offset 0x1AD */  /* Offset 0xA01 */

    U32 rBleExtHdrPrsEn      : 1;               /* Offset 0x1AE */  /* Offset 0xA02 */
    U32 _reserved_bits_A02h  : 15;              /* Offset 0x1AE */  /* Offset 0xA02 */

    tBt3RxCtl rBt3RxCtl;                        /* Offset 0x1B0 */  /* Offset 0x28C */

    U32  rBt3SrchWinCenter    : 12;             /* Offset 0x1B4 */  /* Offset 0x1C8 */
	U32  _reserved_byte_1C9h  : 4;              /* Offset 0x1B5 */  /* Offset 0x1C9 */
    U32  rBt3HalfSrchWin      : 12;             /* Offset 0x1B6 */  /* Offset 0x1CA */
	U32  _reserved_byte_1CBh  : 4;              /* Offset 0x1B7 */  /* Offset 0x1CB */

    t_RxBuf rRxBuf ;            /* Offset 0x1B8~ Offset 0x1DB */ /* Offset 0x2AC~ Offset 0x2CF */

    U32 rIntrMask;                               /* Offset 0x1DC */  /* Offset 0x294 */

    U32 rBtSwIntSet          : 1;                /* Offset 0x1E0 */  /* Offset 0x910 */
    U32 _reserved_bits_910h  : 31;               /* Offset 0x1E0 */  /* Offset 0x910 */

    U32 rIntrStatus;                             /* Offset 0x1E4 */  /* Offset 0x298 */

    U16  rErrorStatus;                           /* Offset 0x1E8 */  /* Offset 0x240 */
    U16  _reserved_word_242h;                    /* Offset 0x1EA */  /* Offset 0x242 */

    U32 rScanDurationClk;                        /* Offset 0x1EC */  /* Offset 0xA70 */
    U32 rScanDurationPhase   : 12;               /* Offset 0x1F0 */  /* Offset 0xA74 */
    U32 _reserved_dword_A74h : 20;               /* Offset 0x1F0 */  /* Offset 0xA74 */
    U32 rScanClk;                                /* Offset 0x1F4 */  /* Offset 0xA78 */
    U32 rScanPhase           : 12;               /* Offset 0x1F8 */  /* Offset 0xA7C */
    U32 _reserved_dword_A7Ch : 20;               /* Offset 0x1F8 */  /* Offset 0xA7C */

    U32 rBt3AcCode2Lo;                           /* Offset 0x1FC */  /* Offset 0x17C */
    U32 rBt3AcCode2Hi;                           /* Offset 0x200 */  /* Offset 0x180 */

    LPS_CTL_STRU rLpsCtl;                        /* Offset 0x204 ~ 0x22F */

    AES_CTL_STRU rAesCtl;                        /* Offset 0x230 ~ 0x24F */

    U32 _reserved_dword_250h[32];                /* Offset 0x250 ~ 0x2CF */

    BD_ADDR_STRU  rBleRcvdTxAddr;                /* Offset 0x2D0 */
    U8   rBleRcvdTxAddrType;                     /* Offset 0x2D6 */
    U8   _reserved_word_61Fh;                    /* Offset 0x2D7 */
    BD_ADDR_STRU  rBleRcvdRxAddr;                /* Offset 0x2D8 */
    U8   rBleRcvdRxAddrType;                     /* Offset 0x2DE */
    U8   _reserved_word_627h;                    /* Offset 0x2DF */
    WHT_CTL_BR_STRU rWhtCtlBr;                   /* Offset 0x2E0 ~ 0x2EF */
    RESO_CTL_BR_STRU  rResoCtlBr;                /* Offset 0x2F0 ~ 0x2FB */

    U32 _reserved_dword_2FCh;                    /* Offset 0x2FC ~ 0x2FF */

    BD_ADDR_STRU  rPubAddr;                      /* Offset 0x300 */  /* Offset 0x14 */
    U16           _rsvd_word_1Ah;                /* Offset 0x306 */  /* Offset 0x1A */
    BD_ADDR_STRU  rRandAddr;                     /* Offset 0x308 */  /* Offset 0x1C */
    U8            rRandAddrEn;                   /* Offset 0x30E */  /* Offset 0x22 */
    U8            _rsvd_byte_23h;                /* Offset 0x30F */  /* Offset 0x23 */

    U32  rBleCmdWord;                            /* Offset 0x310 */  /* Offset 0x23C */
    tBleMiscCtl rBleMiscCtl;                     /* Offset 0x314 */   /* Offset 0x3EC */

    U32  rBleSrchWinCenter    : 12;              /* Offset 0x318 */  /* Offset 0x1CC */
    U32  _reserved_byte_1CDh  : 4;               /* Offset 0x319 */  /* Offset 0x1CD */
    U32  rBleHalfSrchWin      : 12;              /* Offset 0x31A */  /* Offset 0x1CE */
	U32  _reserved_byte_1CFh  : 4;               /* Offset 0x31B */  /* Offset 0x1CF */

    U32 rBleTxExtHdrSrcAddr;                     /* Offset 0x31C */  /* Offset 0xA50 */
    U32 rBleRxExtHdrDstAddr;                     /* Offset 0x320 */  /* Offset 0xA60 */
    U32 rBleCmdArbClk;                           /* Offset 0x324 */  /* Offset 0xA80 */
    U32 rBleCmdArbPhase      : 12;               /* Offset 0x328 */  /* Offset 0xA84 */
    U32 _reserved_dword_A84h : 20;               /* Offset 0x328 */  /* Offset 0xA84 */

    WLCTL_STRU  rWlCtl;                          /* Offset 0x32C~ 0x34F */  /* Offset 0x500 ~ 0x523 */
    DIACTL_STRU  rDiaCtl;                        /* Offset 0x350~ 0x373 */  /* Offset 0x600 ~ 0x623 */
    RACTL_STRU  rRaCtl;                          /* Offset 0x374~ 0x393 */  /* Offset 0x700~ 0x0x71F */

    LE_ADDR_REG_STRU rBleXpctedTxAddr[3];               /* Offset 0x394 */
    LE_ADDR_REG_STRU rBlePeerTxAddr;                    /* Offset 0x3AC */
    U32  rBleXpctedTxAddrEn  : 3;                       /* Offset 0x3B4 */
    U32  rBlePeerTxAddrEn    : 1;                       /* Offset 0x3B4 */
    U32 _reserved_dword_3B4h : 28;                      /* Offset 0x3B4 */
    U32  rBleXpctedTxAddrMatchIdx : 2;                  /* Offset 0x3B8 */
    U32 _reserved_dword_3B8h  : 30;                     /* Offset 0x3B8 */

    LE_ADDR_REG_STRU rBleXpctedRxAddr[3];               /* Offset 0x3BC */
    LE_ADDR_REG_STRU rBlePeerRxAddr;                    /* Offset 0x3D4 */

    U32  rBleXpctedRxAddrEn  : 3;                       /* Offset 0x3DC */
    U32  rBlePeerRxAddrEn    : 1;                       /* Offset 0x3DC */
    U32 _reserved_byte_EC8h  : 28;                      /* Offset 0x3DC */
    U32  rBleXpctdRxAddrMatchIdx : 2;                   /* Offset 0x3E0 */
    U32 _reserved_byte_3E0h  : 30;                      /* Offset 0x3E0 */

    U32 _reserved_dword_3E4h ;                    /* Offset 0x3E4 */

    BLE_HOP_MANUAL_CTL rBleHopManualCtl;         /* Offset 0x3E8 ~ 0x40B */
    BLE_MANUAL_HOP_STATUS rBleManHopStatus;      /* Offset 0x404 ~ 0x40B */

    U32 _reserved_dword_40Ch[61];                 /* Offset 0x40C ~ 0x4FF */
    INQUIRY_CTL_STRU rInquiryCtl;                 /* Offset 0x500 ~ 0x503 */
    INQUIRY_SCAN_CTL_STRU  rInquiryScanCtl;       /* Offset 0x504 ~ 0x507 */
    PAGE_CTL_STRU rPageCtl;                       /* Offset 0x508 ~ 0x50B */
    PAGE_SCAN_CTL_STRU rPageScanCtl;              /* Offset 0x50C ~ 0x50F */

    tFhsCfg rFhsCfg;                              /* Offset 0x510-0x517 */

//    U32  Dummy_518h[58];                        /* Offset 0x518~0x593 */
    AUDIO_CTL_STRU rAudioCtl;

    U32 rAuTxIntMask         : 1;            /* Offset 0x594 */      /* Offset 0x900 */
    U32 _reserved_bits_900h  : 31;           /* Offset 0x594 */      /* Offset 0x900 */
    U32 rAuTxIntFlag         : 1;            /* Offset 0x598 */      /* Offset 0x904 */
    U32 _reserved_bits_904h  : 31;           /* Offset 0x598 */      /* Offset 0x904 */
    U32 rAuRxIntMask         : 1;            /* Offset 0x59C */      /* Offset 0x908 */
    U32 _reserved_bits_908h  : 31;           /* Offset 0x59C */      /* Offset 0x908 */
    U32 rAuRxIntFlag         : 1;            /* Offset 0x5A0 */      /* Offset 0x90C */
    U32 _reserved_bits_90Ch  : 31;           /* Offset 0x5A0 */      /* Offset 0x90C */

    U32 _reserved_dword_5A4h[23];                 /* Offset 0x5A4~0x600 */

    U16  rBt3RxCmdRslt;                           /* Offset 0x600 */
    U16  rBt3TxCmdRslt;                           /* Offset 0x602 */
    U16  rBleRxCmdRslt;                           /* Offset 0x604 */
    U16  rBleTxCmdRslt;                           /* Offset 0x606 */
    U16  rBt3RxPktType;                           /* Offset 0x608 */
    U16  _reserved_word_60Ah;                     /* Offset 0x60A */
    U16  rBleRxPktType;                           /* Offset 0x60C */
    U16  _reserved_word_60Eh;                     /* Offset 0x60E */

    U16  rBt3RxPktHdr;                            /* Offset 0x610 */
    U16  rBt3RxPldHdr;                            /* Offset 0x612 */
    U16  rBleRxPduHdr;                            /* Offset 0x614 */
    U16  _reserved_word_616h;                     /* Offset 0x616 */

    U32  _reserved_word_618h[4];
    #if 0
    BD_ADDR_STRU  rBleRcvdTxAddr;                 /* Offset 0x618 */
    U8   rBleRcvdTxAddrType;                      /* Offset 0x61E */
    U8   _reserved_word_61Fh;                     /* Offset 0x61F */
    BD_ADDR_STRU  rBleRcvdRxAddr;                 /* Offset 0x620 */
    U8   rBleRcvdRxAddrType;                      /* Offset 0x626 */
    U8   _reserved_word_627h;                     /* Offset 0x627 */
    #endif

    U8  rBleRxDiamIdx;                            /* Offset 0x628 */
    U8  _reserved_byte_629h;                      /* Offset 0x629 */
    U16 _reserved_word_62Ah;                      /* Offset 0x62A */

    tRmtFhs rRmtFhs;                              /* Offset 0x62C - 0x64B */
    U32 rBt3MiscStatus;                           /* Offset 0x64C */
	U32  rBt3PktEndNativeClk   : 28;              /* Offset 0x650 */
	U32  _reserved_bits_653h   : 4;               /* Offset 0x653 */

	U32  rBt3PktEndNativePhase : 12;               /* Offset 0x654 */
	U32  _reserved_dword_654h  : 20;               /* Offset 0x654 */
	U32  rBlePktEndNativeClk   : 28;               /* Offset 0x658 */
	U32  _reserved_dword_658h  : 4;                /* Offset 0x658 */

	U32  rBlePktEndNativePhase : 12;               /* Offset 0x65C */
	U32  _reserved_dword_65Ch  : 20;               /* Offset 0x65C */

	U32  rLatchedSyncNativeClk;                    /* Offset 0x660 */
	U16  rLatchedSyncNativePhase;                  /* Offset 0x664 */
	U16  _reserved_word_666h;                      /* Offset 0x666 */

	U32  rSyncPicoPhaseOffset  : 12;               /* Offset 0x668 */
	U32  _reserved_dword_668h  : 20;               /* Offset 0x668 */

    U32 rPrbs_Ctrl_En;                             /* Offset 0x66C */
    U32 rPrbs_Val;                                 /* Offset 0x670 */

    U32 _reserved_dword_674h[3];                   /* Offset 0x674~0x67c */

    rTCTAB rTcTab;                                  /* Offset 0x680 */

    U8   rCtxCtl;                                      /* Offset 0x68C */
    U8   rContModeEnable;                              /* Offset 0x68D */
    U16  _reserved_word_68Eh;                          /* Offset 0x68E */

    U32 _reserved_dword_690h;                          /* Offset 0x690 */

    U32 rTRxTimingDbg;                                  /* Offset 0x694 */

    U32 rBt3Dbg0;                                       /* Offset 0x698 */
    U32 rBt3Dbg1;                                       /* Offset 0x69C */
    U32 rBt3Dbg2;                                       /* Offset 0x6A0 */
    U32 rBt3Dbg3;                                       /* Offset 0x6A4 */
    U32 rBt3DbgErrFlag;                                 /* Offset 0x6A8 */
    U32 rBleDbg0;                                       /* Offset 0x6AC */

    U32 _reserved_dword_6B0h[20];                       /* Offset 0x6B0~0x6FF */

    t_rTrxManualEnCtl  rTrxManualEnCtl;                 /* Offset 0x700~0x703 */

    U8 rTrxManualChannel;                              /* Offset 0x704 */ /* Offset 0x45C */
    t_rTrxManualCtl rTrxManualCtl;                     /* Offset 0x705 */ /* Offset 0x45D */
    U8 rTrxManualHopDone    : 1;                       /* Offset 0x706 */ /* Offset 0x45E */
    U8 rBgLdoSwEn           : 1;   //- bit 17
    U8 rBbPllSwEn           : 1;   //- bit 18
    U8 rBbPllRdySwEn        : 1;   //- bit 19
    U8 rBbAdcSwEn           : 1;   //- bit 20
    U8 rBbDacSwEn           : 1;   //- bit 21
    U8 rBbPllCkSelSwEn      : 1;   //- bit 22
    U8 _reserved_byte_706h  : 1;   //- bit 23

    U8 rTrxManualIsRx       : 1;                       /* Offset 0x707 */   /* Offset 0x45F */
    U8 rTrxManualIsBleMode  : 1;                       /* Offset 0x707 */   /* Offset 0x45F */
    U8 _reserved_byte_707h  : 6;   //- bit 26 - bit 31

    U8 rBgLdoSwAppend       : 1;                      /* Offset 0x708, bit 0 */
    U8 rBbPllSwAppend       : 1;                      /* Offset 0x708, bit 1 */
    U8 rBbPllRdySwAppend    : 1;                      /* Offset 0x708, bit 2 */
    U8 _reserved_byte_708h : 5;
    U8 _reserved_byte_709h[3];                        /* Offset 0x709~0x70B */

    U32 _reserved_dword_70Ch[9];                      /* Offset 0x70C~0x72F */

    BT3_TXFHS_MAN_STRU  rTxFhsMan;                    /* Offset 0x730~0x733 */

    U32 _reserved_dword_734h[115];                    /* Offset 0x734~0x8FF */

    U32 rFpgaHwRst;                                   /* Offset 0x900 */

    U32 _reserved_dword_904h[4];                      /* Offset 0x904 ~ 0x913 */

    RESO_CTL_STRU rResoCtl;                           /* Offset 0x914 ~ 0x95B */
    WHT_CTL_STRU rWhtCtl;                             /* Offset 0x95C ~ 0x96F */

} t_hardware_baseband_registers;

typedef struct stru_ull_clock_information {
    BTCLK rRxClkOffset;
    BTPHASE rRxPhsOffset;
    U8 rValid;
    U8 rsvd;
} ULL_CLK_INFO, *ULL_CLK_INFO_PTR;

EXTERN VOLATILE t_hardware_baseband_registers *rBb;
EXTERN VOLATILE ULL_CLK_INFO_PTR rBb_ull;

#endif /* __SFR_BT_H__ */

