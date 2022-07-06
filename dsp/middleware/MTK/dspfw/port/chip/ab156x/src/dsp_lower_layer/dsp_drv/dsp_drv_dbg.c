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
 
#include "config.h"
////#include "os.h"
////#include "rc.h"
//#include "sfr_au_codec.h"
#include "DSP_Drv_codec.h"

#include "dsp_drv_dbg.h"
#include "dsp_drv_dfe.h"
////#include "os_intr_lv3.h"




/******************************************************************************
 * Function Definitions
 ******************************************************************************/


/******************************************************************************
 * Function Declaration
 ******************************************************************************/


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/


/******************************************************************************
 * DSP Commands Handler Vector
 ******************************************************************************/

 /******************************************************************************
  * Variables
  ******************************************************************************/
/*
 S16 DBG_InBuf[DBG_BUF_SIZE];//Shall be DWaligned
 S32_OUTDATA_t DBG_DumpOutL[DUMP_HALF_BUFSIZE*2];
 S32_OUTDATA_t DBG_DumpOutR[DUMP_HALF_BUFSIZE*2];
 S32 DBG_tempOut[DUMP_HALF_BUFSIZE*2];
 S32 DBG_ID_MASK[2];
 S32 DataOut_LR_Key; // 0 : Lch 1 : Rch
 DBG_CTRL_STRU DBG_ctrl;
 U16 DataOutFlag;
 */
 U16 Dump_enable = 0;


VOID DUMP_DBG_INIT(VOID)
{
/*     DUMP_DBG_SFR_INIT(Dump_I2S_MS_Out);
    DUMP_DBG_BUFFER_INIT();
    if (I2S.CTL0.field.EN_I2S0_MOD == 0) // ENABLE I2S MS
    {
        DSP_DRV_I2S_MS_INIT(I2S_FS_RATE_96K,I2S_WORD_LEN_32BIT,I2S_WORD_LEN_32BIT,I2S_WORD_LEN_32BIT, I2S_TX_MODE);
        SYSTEM.CLKEN.field.i2s_clk_en        = 1;  // system sfr: i2s clk en
    }
    DataOut_LR_Key = 0;
    OS_LV3_INTR_RegisterHandler(OS_LV3_INTR_ID_ODFE_DBG,DSP_oDFE_DBGIsrHandler, DSP_TASK_ID);
    Dump_enable = 1; */
}

VOID DUMP_DBG_SFR_INIT( Dump_output_sel_enum_s DUMP_OUTPUT_SEL)
{
 /*    AUDIO_DFE.CTL1.field.RST_AU_DBG_CH           = 1;
    DSP_DRV_RADMA_INIT (ADMA_CH3 , (U8*)DBG_DumpOutL,  (U32)DUMP_HALF_BUFSIZE*2,  (U32)DUMP_HALF_BUFSIZE);
    DSP_DRV_RADMA_INIT (ADMA_CH4 , (U8*)DBG_DumpOutR,  (U32)DUMP_HALF_BUFSIZE*2,  (U32)DUMP_HALF_BUFSIZE);
    AUDIO_DFE.OUT_SET0.field.DBG_BUF_THRP_RATE_SEL    = DUMP_OUTPUT_SEL;

    switch (DUMP_OUTPUT_SEL)
    {
        case 0 :
        AUDIO_DFE.OUT_SET0.field.ODFE_THRP_RATE_SEL = 0;// Cant find path on HW Architecture
        break;
        case 1 :
        AUDIO_DFE.OUT_SET0.field.I2S_MASTER_SDO_MUX = 1;
        break;
        case 2 :
        AUDIO_DFE.OUT_SET0.field.I2S_SLAVE_SDO_MUX = 1;
        break;
        case 3 :
        AUDIO_DFE.OUT_SET0.field.SPDIF_TX_MUX = 1;
        break;
        case 4 : // USB dump reserved
        break;
        default :
        configASSERT(0);
        break;
    }
    AUDIO_DFE.CTL0.field.EN_AU_DBG_CH    = 1; */
}

VOID DUMP_DBG_BUFFER_INIT (VOID)
{
/*     memset(DBG_InBuf,0,DBG_BUF_SIZE*4);
    memset(DBG_DumpOutL,0,DUMP_HALF_BUFSIZE*2*4);
    memset(DBG_DumpOutR,0,DUMP_HALF_BUFSIZE*2*4); */
}

void Dump_DebugIntrHandler (void)
{
/*     Debug_DataOut(); */
}


void Debug_DataIn (S16* InBuf, U16 DataLength, U16 DebugID)
{
/*     U16 i;
    U16 ID_offset;

    if (Dump_enable == 0)
    {
    return;
    }

    if (DebugID>=32)
    {
        ID_offset = 1;
        DebugID = DebugID%32;
    }
    else
    {
        ID_offset = 0;
    }
    //if ((DBG_ID_MASK[ID_offset] & (1 << DebugID)) == 0)//Now is all ID pass
    {
        while((DBG_BUF_SIZE - DBG_ctrl.BufCnt - 2) < DataLength )
        {
            configASSERT(0);
        }
        DBG_InBuf[DBG_ctrl.BufWo]      = DebugID;
        DBG_ctrl.BufWo = (DBG_ctrl.BufWo + 1) % DBG_BUF_SIZE;
        DBG_InBuf[DBG_ctrl.BufWo]      = DataLength;
        DBG_ctrl.BufWo = (DBG_ctrl.BufWo + 1) % DBG_BUF_SIZE;

        // DATA WRITE START
        //Is_Length_Odd = (DataLength&1);
        for (i = 0; i<DataLength; i++)
        {
            DBG_InBuf[(DBG_ctrl.BufWo+i) % DBG_BUF_SIZE] = InBuf[i];
        }
        DBG_ctrl.BufWo  = (DBG_ctrl.BufWo + DataLength) % DBG_BUF_SIZE;
        DBG_ctrl.BufCnt = (DBG_ctrl.BufCnt + 2 + DataLength);
        //OS_EXIT_CRITICAL(ps);

    } */

}


void Debug_DataOut (void)
{


/*     U16 DbgDataIdxL;
    U16 DbgDataIdxR;
    U16 LInterruptStatus;
    U16 RInterruptStatus;

    if ((Dump_enable == 0)||(DataOutFlag ==0))
    {
    return;
    }

    LInterruptStatus = AU_ODFE_CH3_RADMA.STAT.field.INTR_TOKEN;
    memset(&DBG_DumpOutL[(LInterruptStatus^1)*(DUMP_HALF_BUFSIZE)],0,DUMP_HALF_BUFSIZE*4);
    RInterruptStatus = AU_ODFE_CH4_RADMA.STAT.field.INTR_TOKEN;
    memset(&DBG_DumpOutR[(RInterruptStatus^1)*(DUMP_HALF_BUFSIZE)],0,DUMP_HALF_BUFSIZE*4);


    DbgDataIdxL = 0;
    DbgDataIdxR = 0;
    while (((DbgDataIdxL+DbgDataIdxR) < DUMP_HALF_BUFSIZE) && (DBG_ctrl.BufCnt))
    {
        if (!DBG_ctrl.DUMP_IDRemainSize)
        {
            DBG_ctrl.DUMP_ID            = DBG_InBuf[DBG_ctrl.BufRo];
            DBG_ctrl.BufRo              = (DBG_ctrl.BufRo + 1) % DBG_BUF_SIZE;
            DBG_ctrl.DUMP_IDRemainSize  = DBG_InBuf[DBG_ctrl.BufRo];
            DBG_ctrl.BufRo              = (DBG_ctrl.BufRo + 1) % DBG_BUF_SIZE;
            DBG_ctrl.BufCnt             -= 2;
            continue;
        }
        if (DataOut_LR_Key == 0)
        {
            DBG_DumpOutL[(LInterruptStatus^1)*(DUMP_HALF_BUFSIZE)+DbgDataIdxL].Field.DebugID   = DBG_ctrl.DUMP_ID;
            DBG_DumpOutL[(LInterruptStatus^1)*(DUMP_HALF_BUFSIZE)+DbgDataIdxL].Field.OutData   = DBG_InBuf[DBG_ctrl.BufRo];
            DataOut_LR_Key = 1;
            DbgDataIdxL++;
        }
        else
        {
            DBG_DumpOutR[(RInterruptStatus^1)*(DUMP_HALF_BUFSIZE)+DbgDataIdxR].Field.DebugID   = DBG_ctrl.DUMP_ID;
            DBG_DumpOutR[(RInterruptStatus^1)*(DUMP_HALF_BUFSIZE)+DbgDataIdxR].Field.OutData   = DBG_InBuf[DBG_ctrl.BufRo];
            DataOut_LR_Key = 0;
            DbgDataIdxR++;
        }

        DBG_ctrl.BufRo              = (DBG_ctrl.BufRo + 1) % DBG_BUF_SIZE;
        DBG_ctrl.BufCnt--;
        DBG_ctrl.DUMP_IDRemainSize--;
    }
    DataOutFlag = 0; */
}

VOID DSP_oDFE_DBGIsrHandler(VOID)
{
    // if ((AU_ODFE_CH3_RADMA.STAT.field.THD_INTR)||(AU_ODFE_CH3_RADMA.STAT.field.THD_INTR))
    // {

        // configASSERT(DataOutFlag == 0);
        // DataOutFlag = 1;
        // AU_ODFE_CH3_RADMA.CTL.field.THD_INTR_CLR = 1; /* Clear Audio Interrupt Source */
        // AU_ODFE_CH4_RADMA.CTL.field.THD_INTR_CLR = 1; /* Clear Audio Interrupt Source */
    // }
}

VOID DUMP_DBG_DISABLE(VOID)
{
/*     DUMP_DBG_BUFFER_INIT();
    DataOut_LR_Key = 0;
    DSP_DRV_I2S_MS_END();
    DSP_DRV_RADMA_END(ADMA_CH3);
    DSP_DRV_RADMA_END(ADMA_CH4);
    OS_LV3_INTR_CancelHandler(OS_LV3_INTR_ID_ODFE_DBG);
    AUDIO_DFE.CTL1.field.RST_AU_DBG_CH   = 1;
    AUDIO_DFE.CTL0.field.EN_AU_DBG_CH     = 0;
    Dump_enable = 0; */
}

