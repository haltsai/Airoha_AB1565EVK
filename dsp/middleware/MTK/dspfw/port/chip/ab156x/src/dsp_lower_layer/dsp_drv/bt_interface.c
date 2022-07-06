/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#include "dsp_temp.h"
#include "bt_interface.h"

#define BTCLK_LEN                   4
#define STATE_LEN                   4
#define VOICE_HEADER                (BTCLK_LEN + STATE_LEN)
#define ESCO_UL_ERROR_DETECT_THD    (8)
#define BT_FRAME_UNIT               (2500)
#define BT_NCLK_MASK                (0x0FFFFFFC)
#define BT_SLOT_UNIT                (625)

VOLATILE t_hardware_baseband_registers *rBb = (VOLATILE t_hardware_baseband_registers *)0xB0010000;
VOLATILE ULL_CLK_INFO_PTR rBb_ull = NULL;

VOID MCE_BtClkPhaseSwitch(BTCLK* pCLK, BTPHASE* pPhase)
{
    if (*pPhase >= 2500)
    {
        *pPhase -= 2500;
        *pCLK += 4;
    }

    if (*pPhase < 1250)
    {
        if (*pPhase >= 625)
        {
            *pCLK += 0x01;
            *pPhase -= 625;
        }
    }
    else if (*pPhase < 1875)
    {
        *pCLK += 0x02;
        *pPhase -= 1250;
    }
    else
    {
        *pCLK += 0x03;
        *pPhase -= 1875;
    }
}


VOID MCE_Update_BtClkOffsetAddr(void *addr,BT_CLOCK_OFFSET_SCENARIO type)
{
    if(type == ULL_CLK_Offset){
        rBb_ull = (ULL_CLK_INFO_PTR)addr;
    }else{
        DSP_MW_LOG_E("[MCE]Update wrong BT offset address",0);
    }
}

VOID MCE_Get_BtClkOffset(BTCLK* pClkOffset, BTPHASE* pNClkOffse,BT_CLOCK_OFFSET_SCENARIO type)
{
    BTPHASE    PhaseOffset;
    BTCLK      ClockOffset;
    if(type == BT_CLK_Offset){
        PhaseOffset = rBb->rAudioCtl.rRxPhsOffset;
        ClockOffset = rBb->rAudioCtl.rRxClkOffset;
    }else if(type == ULL_CLK_Offset){
        if(rBb_ull->rValid){
            PhaseOffset = rBb_ull->rRxPhsOffset;
            ClockOffset = rBb_ull->rRxClkOffset;
        }else{
            DSP_MW_LOG_W("[MCE] rValid = 0,maybe get the worng clock offset",0);
        }
    }else{
        DSP_MW_LOG_E("[MCE]Get wrong BT offset type",0);
    }
    
    *pClkOffset = ClockOffset;
    *pNClkOffse = PhaseOffset;
}

VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type)
{
    BTCLK CurrCLK,NativeCLK;
    BTPHASE CurrPhase;

    BTPHASE    PhaseOffset;
    BTCLK      ClockOffset;
    
    MCE_Get_BtClkOffset(&ClockOffset,&PhaseOffset,type);

    do{
        NativeCLK = rBb->rClkCtl.rNativeClock&0x0FFFFFFC; /*Native Clk*/
        CurrCLK = (NativeCLK + ClockOffset)&0x0FFFFFFC;   /*Bt Clk*/
        CurrPhase = rBb->rClkCtl.rNativePhase + PhaseOffset; /*Bt Intra Clk*/
    } while(NativeCLK != (rBb->rClkCtl.rNativeClock&0x0FFFFFFC));

    MCE_BtClkPhaseSwitch(&CurrCLK, &CurrPhase);

    *pCurrCLK = CurrCLK;    /*Bt Clk*/
    *pCurrPhase = CurrPhase;/*Bt Intra Clk*/
}

VOID MCE_Get_NativeClk_from_Controller(U32* pNClk, U32* pAClk)
{
    U32    NativeClk = rBb->_reserved_dword_904h[2];
    U32    AudioClk = rBb->_reserved_dword_904h[3];

    *pNClk = NativeClk;
    *pAClk = AudioClk;
    //printf("NativeClk:0x%x, AudioClk:0x%x",NativeClk, AudioClk);
}

VOID MCE_Initial_Aud_Cnt_from_Controller(void)
{
    rBb->_reserved_dword_904h[3] = 0;
}

VOID MCE_TransBT2NativeClk(BTCLK CurrCLK, BTPHASE CurrPhase,BTCLK* pNativeBTCLK, BTPHASE* pNativePhase,BT_CLOCK_OFFSET_SCENARIO type)
{
    BTPHASE    PhaseOffset;
    BTCLK      ClockOffset;

    MCE_Get_BtClkOffset(&ClockOffset,&PhaseOffset,type);

    if (CurrPhase > BT_FRAME_UNIT) {
        CurrPhase -= (BT_FRAME_UNIT);
        CurrCLK += 4;
    }
    *pNativeBTCLK = (CurrCLK - ClockOffset);
    *pNativePhase = (CurrPhase - PhaseOffset);
    if (*pNativePhase > BT_FRAME_UNIT) {
        *pNativePhase += (BT_FRAME_UNIT);
        *pNativeBTCLK -= 4;
    }
    uint32_t remain_n = *pNativeBTCLK & 0x03;
    if (remain_n != 0) {
       uint32_t intra_2 = remain_n * BT_SLOT_UNIT + *pNativePhase;
       if (intra_2 > BT_FRAME_UNIT) {
            intra_2 -= BT_FRAME_UNIT;
            *pNativeBTCLK += 4;
            *pNativePhase = intra_2;
       }
    }
    *pNativeBTCLK &= BT_NCLK_MASK;
}


VOID MCE_LatchSrcTiming (VOID)
{
    BTCLK BtClk;
    BTPHASE BtPhase;

    MCE_GetBtClk(&BtClk, &BtPhase,BT_CLK_Offset);
    DSP_MW_LOG_I("BtClk:%d, BtPhase:%d \r\n", 2, BtClk, BtPhase);
}


VOID AT_MCE_LatchSrcTiming (hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    BTCLK BtClk;
    BTPHASE BtPhase;

    MCE_GetBtClk(&BtClk, &BtPhase,BT_CLK_Offset);
    DSP_MW_LOG_I("BtClk:%d, BtPhase:%d\r\n", 2, BtClk, BtPhase);
}


