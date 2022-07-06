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
#include "dsp_feature_interface.h"
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_audio_ctrl.h"
#include "dsp_share_memory.h"
#include "aec_nr_para.h"
#include "aec_nr_interface.h"
#include "audio_nvdm_common.h"
#include "dsp_dump.h"
#include "string.h"
#include "dsp_memory.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_msg.h"

//#include "DSP_DataDump.h"
#include "dsp_callback.h"
//#include "dsp_memory.h"

#ifdef MTK_BT_A2DP_ECNR_USE_PIC
#include "ecnr_portable.h"
#endif
#ifdef MTK_3RD_PARTY_NR
#ifdef AIR_IGO_NR_PLUS_1_ENABLE
#include "anc_api.h"
#endif
#endif

#ifdef AIR_EC120_ENABLE
#include "ec120_portable.h"
#endif

/**
 *
 *  Definition
 *
 */
#define AEC_WB_FRAME_SIZE                 ((U32)480)
#define AEC_NB_FRAME_SIZE                 ((U32)240)
#define AEC_75ms_FRAME_SIZE               ((U32)240)
#define AEC_VALID_MEMORY_CHECK_VALUE      ((U32)0x6128800)
#define AEC_RUNTIME_INVALID_MEMORY_CHECK_VALUE    ((U32)0x5A5A5A5A)
#if defined(AIR_EC120_ENABLE)
#define EC120_VALID_MEMORY_CHECK_VALUE      ((U32)0x6487647)
#define EC120_RUNTIME_INVALID_MEMORY_CHECK_VALUE    ((U32)0x95279527)
#endif

#define AEC_FORCE_WB_PROCEDURE          (false)

#define AEC_REF_GAIN_SIZE                 (8)

#ifdef MTK_AIRDUMP_EN
#define AEC_PRELIM_COEF_SIZE              (9)
#define AEC_DUALMIC_POWER_COEF_SIZE       (1)
#define AEC_INEAR_SIZE                    (6)

#if defined (MTK_INEAR_ENHANCEMENT) || defined (MTK_DUALMIC_INEAR)
#define AEC_AIRDUMP_FRAME_SIZE (AEC_PRELIM_COEF_SIZE+AEC_REF_GAIN_SIZE+AEC_DUALMIC_POWER_COEF_SIZE+AEC_INEAR_SIZE)
#else
#define AEC_AIRDUMP_FRAME_SIZE (AEC_PRELIM_COEF_SIZE+AEC_REF_GAIN_SIZE)
#endif

#define AEC_AIRDUMP_FRAME_NUM             (10)
#endif

#ifdef MTK_INEAR_ENHANCEMENT
#define INEAR_G_F_WIND_DEFAULT_VALUE (0xFFFF) //S16
#define INEAR_MIC_STATUS_O_DEFAULT_VALUE (0xFFFF) //S16
#endif

#if (AEC_TEST_DEFAULT_PARA)
extern const DSP_PARA_AEC_NR_STRU ECNR_DspParaInit;
extern const const DSP_PARA_WB_TX_EQ_STRU WBtxEQ_DspParaInit;
extern const DSP_PARA_WB_RX_EQ_STRU WBrxEQ_DspParaInit;
extern const const DSP_PARA_NB_TX_EQ_STRU NBtxEQ_DspParaInit;
extern const const DSP_PARA_NB_RX_EQ_STRU NBrxEQ_DspParaInit;
#endif
/**
 *
 *  Type Definition
 *
 */


/**
 *
 *  Buffer & Control
 *
 */

CPD_EC_NR_INSTANCE_PTR AecMemoryPtr = NULL;
#ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
static bool gEcIn_1st_prc = true;
static bool gEcInBufIdx = false;
#endif
#if defined(AIR_EC120_ENABLE)
EC120_INSTANCE_PTR Ec120MemoryPtr = NULL;
static bool gDnnIn_1st_prc = true;
static bool gDnnInBufIdx = false;
#endif
bool gfgAvcUpdate;
bool gfgAvcSend;
S16 gi2AvcVol;
#ifdef MTK_AIRDUMP_EN
AIRDUMPCTRL_t * rAirDumpCtrl;
AIRDUMP_DSP_CTRL_t rAirDumpDSPCtrl;
#endif
static bool gAecNrEn_MP = TRUE;
static bool gNeedChangeRxEQ = FALSE;
static int gNRnum;

/**
 *
 * External Symbols
 *
 */

#ifndef MTK_BT_A2DP_ECNR_USE_PIC
EXTERN void EC_REF_GAIN_READBACK (S16 *gain);
#endif

void* p_ecnr_bss;
void* p_ecnr_NvKey;
void* p_txeq_NvKey;
void* p_rxeq_NvKey;
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
void* p_InEarEq_NvKey;
#endif
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
void* p_AstEq_NvKey;
#endif
#ifdef MTK_3RD_PARTY_NR
void* p_reserved_NvKey;
#endif
bool g_ecnr_prev_process_enable = false;

/**
 *
 * Function Prototype
 *
 */
bool stream_function_aec_nr_initialize (void *para);

BOOL AEC_NR_MemCheck (VOID);
BOOL AEC_NR_MemInit (VOID* para, uint32_t alloc_size);
bool stream_function_aec_process (void *para);
bool stream_function_nr_process (void *para);
BOOL AEC_NR_NVKEY_Init (SCO_CODEC mode);
bool avc_vol_compare(S16 i2PreAvcVol, S16 i2PostAvcVol);
VOID aec_nr_get_rx_eq_nvkey(U16 nvkey_0, U16 nvkey_1, VOID *p_nvkey_dst, U32 length);

#if defined(AIR_EC120_ENABLE)
bool EC_120_MemCheck (void);
bool EC_120_MemInit (void* para);
bool stream_function_ec_120_initialize (void *para);
bool stream_function_ec120_process (void *para);
#endif

static bool AEC_NR_Check_Runing_Status(void)
{
    if ((AecMemoryPtr != NULL) && (AecMemoryPtr->MemoryCheck == AEC_VALID_MEMORY_CHECK_VALUE)) {
        return true;
    }
#ifdef AIR_EC120_ENABLE
    else if ((Ec120MemoryPtr != NULL) && (Ec120MemoryPtr->MemoryCheck == EC120_VALID_MEMORY_CHECK_VALUE)) {
        return true;
    }
#endif

    return false;
}

#ifdef MTK_3RD_PARTY_NR
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
static volatile bool g_igo_nr_init_flag = false;
#endif
#ifdef AIR_IGO_NR_PLUS_1_ENABLE
static _SWITCH_MODE g_iGO_NR_Curr_Mode;
static _SWITCH_MODE IGO_NR_Get_Curr_Mode(void)
{
    uint8_t anc_enable;
    dsp_anc_control_type_t anc_type;
    _SWITCH_MODE anc_mode;

    dsp_anc_control_get_status(&anc_enable, &anc_type, NULL);
    if (anc_enable == true) {
        if (anc_type == DSP_ANC_CONTROL_TYPE_PASSTHRU_FF) {
            anc_mode = _PASSTHROUGH;
        } else {
            anc_mode = _ANC_ON;
        }
    } else {
        anc_mode = _ANC_OFF;
    }

    return anc_mode;
}
#endif
#endif

void stream_function_aec_nr_deinitialize (bool is_aec_only)
{
    if (is_aec_only) {
        if (AEC_NR_MemCheck()) {
            AecMemoryPtr->deinit_process = true;
        }
    } else {
    if (AecMemoryPtr) {
            if (AEC_VALID_MEMORY_CHECK_VALUE == AecMemoryPtr->MemoryCheck) {
        AecMemoryPtr->MemoryCheck = AEC_RUNTIME_INVALID_MEMORY_CHECK_VALUE;
    }
        }
    }
}


/**
 * stream_function_aec_nr_initialize
 *
 * This function is used to init memory space for AEC/NR process
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_function_aec_nr_initialize (void *para)
{
    uint32_t alloc_size;
    SCO_CODEC     sco_type;
    DSP_MW_LOG_I("stream_function_aec_nr_initialize start",0);

#if defined(MTK_3RD_PARTY_NR) && defined(AIR_IGO_NR_PLUS_1_ENABLE)
    alloc_size = DSP_AEC_NR_MEMSIZE + get_aec_nr_inear_memsize();
#else
    alloc_size = DSP_AEC_NR_MEMSIZE + get_aec_nr_memsize();
#endif
    if (AEC_NR_MemInit(para, alloc_size) && (!AecMemoryPtr->deinit_process))
    {
        return FALSE;
    }

    p_ecnr_bss      =  &AecMemoryPtr->ScratchMemory[0];
    p_ecnr_NvKey    =  &AecMemoryPtr->NvKey.AecNr;
    p_rxeq_NvKey    =  &AecMemoryPtr->NvKey.RxEq;
    p_txeq_NvKey    =  &AecMemoryPtr->NvKey.TxEq;
    #if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
        p_InEarEq_NvKey = &AecMemoryPtr->NvKey.InearEq;
    #endif /*MTK_INEAR_ENHANCEMENT || MTK_DUALMIC_INEAR*/
    #if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
        p_AstEq_NvKey = &AecMemoryPtr->NvKey.AstEq;
    #endif
    #ifdef MTK_INEAR_ENHANCEMENT
        AecMemoryPtr->prev_g_f_wind = INEAR_G_F_WIND_DEFAULT_VALUE;
        AecMemoryPtr->prev_mic_status_o = INEAR_MIC_STATUS_O_DEFAULT_VALUE;
    #endif /*MTK_INEAR_ENHANCEMENT*/
    #ifdef MTK_3RD_PARTY_NR
        p_reserved_NvKey = &AecMemoryPtr->NvKey.ResKey;
    #endif /*MTK_3RD_PARTY_NR*/
    sco_type = gDspAlgParameter.EscoMode.Rx; //temp setup rx only

    gfgAvcUpdate = false;
    gfgAvcSend = true;
    gNRnum = 0;
    #ifdef MTK_AIRDUMP_EN
        rAirDumpDSPCtrl.airdump_en = false;
    #endif
    gNeedChangeRxEQ = false;

    #ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
    gEcIn_1st_prc = true;
    gEcInBufIdx = true;
    #endif

    switch (sco_type)
    {
        case CVSD:
            #if (!AEC_FORCE_WB_PROCEDURE)
            //Fix CVSD decoder out 16kHz
            AEC_NR_NVKEY_Init(CVSD);

            //Tx Init
            #ifdef MTK_3RD_PARTY_NR
                #ifdef AIR_IGO_NR_PLUS_1_ENABLE
                    Voice_EC_Inear_Init(1, p_ecnr_bss, p_ecnr_NvKey, (S16 *)&AecMemoryPtr->NvKey.AecNr.WB_NR_TX_NOISE_GAIN_LIMITER);
                #else
                    Voice_EC_Init(1, p_ecnr_bss, p_ecnr_NvKey, (S16 *)&AecMemoryPtr->NvKey.AecNr.WB_NR_TX_NOISE_GAIN_LIMITER);
                #endif
            #else /*#ifdef MTK_3RD_PARTY_NR*/
                #ifdef MTK_INEAR_ENHANCEMENT
                    Voice_WB_TX_Inear_Init(1, NULL, p_ecnr_bss, p_ecnr_NvKey, p_txeq_NvKey, p_InEarEq_NvKey, p_AstEq_NvKey);
                #elif defined(MTK_DUALMIC_INEAR)
                    Voice_WB_TX_Inear_Init_V2(1, NULL, p_ecnr_bss, p_ecnr_NvKey, p_txeq_NvKey, p_InEarEq_NvKey, p_AstEq_NvKey);
                #else
                    Voice_WB_TX_Init(1, NULL, p_ecnr_bss, p_ecnr_NvKey, p_txeq_NvKey);
                #endif
            #endif/*#ifdef MTK_3RD_PARTY_NR*/

            //Rx Init
            if (!AecMemoryPtr->deinit_process) {
            WB_RX_NR_init(p_ecnr_bss, p_ecnr_NvKey, p_rxeq_NvKey);
            }
            break;
            #endif

        case mSBC:
            AEC_NR_NVKEY_Init(mSBC);

            //Tx Init
            #ifdef MTK_3RD_PARTY_NR
                #ifdef AIR_IGO_NR_PLUS_1_ENABLE
                    Voice_EC_Inear_Init(0, p_ecnr_bss, p_ecnr_NvKey, (S16 *)&AecMemoryPtr->NvKey.AecNr.WB_NR_TX_NOISE_GAIN_LIMITER);
                #else
                    Voice_EC_Init(0, p_ecnr_bss, p_ecnr_NvKey, (S16 *)&AecMemoryPtr->NvKey.AecNr.WB_NR_TX_NOISE_GAIN_LIMITER);
                #endif
            #else /*#ifdef MTK_3RD_PARTY_NR*/
                 #ifdef MTK_INEAR_ENHANCEMENT
                    Voice_WB_TX_Inear_Init(0, NULL, p_ecnr_bss, p_ecnr_NvKey, p_txeq_NvKey, p_InEarEq_NvKey, p_AstEq_NvKey);
                 #elif defined(MTK_DUALMIC_INEAR)
                    Voice_WB_TX_Inear_Init_V2(0, NULL, p_ecnr_bss, p_ecnr_NvKey, p_txeq_NvKey, p_InEarEq_NvKey, p_AstEq_NvKey);
                 #else
                    Voice_WB_TX_Init(0, NULL, p_ecnr_bss, p_ecnr_NvKey, p_txeq_NvKey);
                 #endif
            #endif/*#ifdef MTK_3RD_PARTY_NR*/

            //Rx Init
            if (!AecMemoryPtr->deinit_process) {
            WB_RX_NR_init(p_ecnr_bss, p_ecnr_NvKey, p_rxeq_NvKey);
            }
            break;

        default:
            return TRUE;
    }

#ifdef MTK_3RD_PARTY_NR
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    g_igo_nr_init_flag = true;
#endif
#endif

#if defined(MTK_INEAR_ENHANCEMENT)
    DSP_MW_LOG_I("[DSP][AEC/NR] Inhouse 1+1 version %d", 1, get_ECNR_SVN());
#elif defined(MTK_DUALMIC_INEAR)
    DSP_MW_LOG_I("[DSP][AEC/NR] Inhouse 2+1 version %d", 1, get_ECNR_SVN());
#elif defined(MTK_3RD_PARTY_NR)
    DSP_MW_LOG_I("[DSP][AEC/NR] 3rd party version %d", 1, get_ECNR_SVN());
#else
    DSP_MW_LOG_I("[DSP][AEC/NR] Inhouse 1/2 version %d", 1, get_ECNR_SVN());
#endif

    return FALSE;
}



/**
 * AEC_NR_FakeInit
 *
 * This function is used to check init memory space for AEC/NR process
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *

BOOL AEC_NR_FakeInit (VOID* para)
{
    UNUSED(para);

    if (p_ecnr_mem == NULL)
    {
        gDspAlgParameter.AecReady = FALSE;
    }

    return FALSE;
}
*/

/**
 * AEC_NR_MemCheck
 *
 * This function is used to check init memory space for AEC/NR process
 *
 *
 * @para : Default parameter of callback function
 * @return : Check result
 */
BOOL AEC_NR_MemCheck (VOID)
{
    if (NULL != AecMemoryPtr)
    {
        if (AEC_VALID_MEMORY_CHECK_VALUE == AecMemoryPtr->MemoryCheck)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * AEC_NR_MemInit
 *
 * This function is used to init memory space for AEC/NR process
 *
 *
 * @para : Default parameter of callback function
 * @return : Initialize result
 */
BOOL AEC_NR_MemInit (VOID* para, uint32_t alloc_size)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    if (!AEC_NR_MemCheck())
    {
        if (!(AecMemoryPtr && (AecMemoryPtr->MemoryCheck == AEC_RUNTIME_INVALID_MEMORY_CHECK_VALUE))) {
            AecMemoryPtr = (CPD_EC_NR_INSTANCE_PTR)DSPMEM_tmalloc(DAV_TASK_ID, alloc_size, stream_ptr);
        }
        AecMemoryPtr->MemoryCheck = AEC_VALID_MEMORY_CHECK_VALUE;

        #if (AEC_EXPAND_BUF)
        AecMemoryPtr->BufExpansion.AecTxId = 0;
        AecMemoryPtr->BufExpansion.AecRxId = 0;
        #endif

        gDspAlgParameter.AecReady = FALSE;
        return FALSE;
    }
    return TRUE;
}



void AEC_NR_Enable(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    typedef enum {
       AEC_NR_Disable   = 0,
       AEC_NR_Enable  = 1,
    } AEC_NR_Enable_MP;

    switch(msg.ccni_message[1])
    {
        case AEC_NR_Disable:
          DSP_MW_LOG_I("[DSP][AEC/NR] Disable", 0);
          gAecNrEn_MP = FALSE;
          break;
        case AEC_NR_Enable:
          DSP_MW_LOG_I("[DSP][AEC/NR] Enable", 0);
          gAecNrEn_MP = TRUE;
          break;
    }
}



/**
 * stream_function_aec_process
 *
 * The main process for TX AEC/NR
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_aec_process (void *para)
{
    U8   Channel_num    = stream_function_get_channel_number(para);
    S16* InBufLeft      = (S16*)stream_function_get_1st_inout_buffer(para);
    S16* InBufRight     = (S16*)stream_function_get_2nd_inout_buffer(para);
#ifdef ENABLE_2A2D_TEST
    S16* InBufMic3;
    S16* InBufMic4;
    switch(Channel_num) {
        case 4:
            InBufMic3  = (S16*)stream_function_get_3rd_inout_buffer(para);
            InBufMic4 = (S16*)stream_function_get_3rd_inout_buffer(para);
            break;
        case 5:
            InBufMic3  = (S16*)stream_function_get_3rd_inout_buffer(para);
            InBufMic4  = (S16*)stream_function_get_4th_inout_buffer(para);
            break;
        default:
            InBufMic3  = (S16*)stream_function_get_3rd_inout_buffer(para);
            InBufMic4 = (S16*)stream_function_get_3rd_inout_buffer(para);
            break;
    }
#endif
    U16 Length          = (U16)stream_function_get_output_size(para);

    S16* OutBuf         = (S16*)AecMemoryPtr->AecOut;
    S16* EC_path_buf;
    S16 i2PreAvcVol;
    S16 i2PostAvcVol;
    hal_ccni_message_t msg;
#ifdef MTK_INEAR_ENHANCEMENT
    S16 g_f_wind;
    S16 mic_status_o;
#endif

#ifdef ENABLE_2A2D_TEST
    if(Channel_num == 2){
        EC_path_buf    = (S16*)stream_function_get_2nd_inout_buffer(para);
    } else if (Channel_num == 3) {
        EC_path_buf    = (S16*)stream_function_get_3rd_inout_buffer(para);
    } else if (Channel_num == 4) {
        EC_path_buf    = (S16*)stream_function_get_4th_inout_buffer(para);
    } else if (Channel_num == 5) {
        EC_path_buf    = (S16*)stream_function_get_5th_inout_buffer(para);
    } else {
        EC_path_buf    = (S16*)stream_function_get_2nd_inout_buffer(para);
    }
#else
    EC_path_buf = (Channel_num == 2) ? (S16*)stream_function_get_2nd_inout_buffer(para) : (S16*)stream_function_get_3rd_inout_buffer(para);
#endif


    if (AEC_NR_MemCheck() == FALSE)
    {
        //configASSERT(0);
        stream_feature_reinitialize(para);
		DSP_MW_LOG_I("[AEC] reinitialize", 0);
        return FALSE;
    }
    if (AecMemoryPtr->deinit_process) {
        DSP_MW_LOG_I("[AEC] de-initialize", 0);
        stream_function_aec_nr_initialize(para);
        AecMemoryPtr->deinit_process = false;
        audio_nvdm_update_status(AUDIO_NVDM_USER_HFP, AUDIO_NVDM_STATUS_POST_CHANGE);
    }

    if ((AecMemoryPtr->NvKey.AecNr.ENABLE == 0) || (gAecNrEn_MP == FALSE))
    {
        //DSP_MW_LOG_I("[AEC] Disable", 0);
        return FALSE;
    }

#ifndef MTK_3RD_PARTY_NR
    if (gNeedChangeRxEQ) {
        EQ_update(p_ecnr_bss, p_rxeq_NvKey);
        gNeedChangeRxEQ = false;
        DSP_MW_LOG_I("aec_nr change RX EQ param", 0);
    }
#endif

    switch (gDspAlgParameter.EscoMode.Tx)
    {
        case CVSD: // Falls through
        case mSBC:
            if (Length != AEC_WB_FRAME_SIZE)
            {
                #ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
                if(Length != AEC_75ms_FRAME_SIZE)
                #endif
                {
                return (Length != 0);
                }
            }
            break;

        default:
            return TRUE;
    }

#ifdef MTK_AIRDUMP_EN
    if((rAirDumpDSPCtrl.airdump_en && AEC_NR_Check2MicEnabled()))
    {
        //DSP_MW_LOG_I("[AirDump][DSP] NR", 0);
        AEC_NR_AirDump();
    }
#endif

    i2PreAvcVol = gDspAlgParameter.AecNr.avc_vol;

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)EC_path_buf, (U32)Length, VOICE_TX_REF);
    switch(Channel_num) {
		case 2:
		    LOG_AUDIO_DUMP((U8*)InBufLeft, (U32)Length, VOICE_TX_MIC_0);
			break;
		case 3:
		    LOG_AUDIO_DUMP((U8*)InBufLeft, (U32)Length, VOICE_TX_MIC_0);
			LOG_AUDIO_DUMP((U8*)InBufRight, (U32)Length, VOICE_TX_MIC_1);
			break;
#ifdef ENABLE_2A2D_TEST
		case 4:
		    LOG_AUDIO_DUMP((U8*)InBufLeft, (U32)Length, VOICE_TX_MIC_0);
			LOG_AUDIO_DUMP((U8*)InBufRight, (U32)Length, VOICE_TX_MIC_1);
			LOG_AUDIO_DUMP((U8*)InBufMic3, (U32)Length, VOICE_TX_MIC_2);
			break;
	    case 5:
		    LOG_AUDIO_DUMP((U8*)InBufLeft, (U32)Length, VOICE_TX_MIC_0);
			LOG_AUDIO_DUMP((U8*)InBufRight, (U32)Length, VOICE_TX_MIC_1);
			LOG_AUDIO_DUMP((U8*)InBufMic3, (U32)Length, VOICE_TX_MIC_2);
			LOG_AUDIO_DUMP((U8*)InBufMic4, (U32)Length, VOICE_TX_MIC_3);
			break;
#endif
	} /*end of switch*/
#endif

/// For 7.5ms AFE buffering ////////////////////////////////////////////////////////////////
#ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
    U8* AEC_IN_BUF1    = (U8*)AecMemoryPtr->AecInTempBuf1;
    U8* AEC_IN_BUF2    = (U8*)AecMemoryPtr->AecInTempBuf2;
    U8* AEC_IN_BUF3    = (U8*)AecMemoryPtr->AecInTempBuf3;
    U8* AEC_IN_BUFec   = (U8*)AecMemoryPtr->AecInTempBufec;
    U8  ECNR_OUT_idx = 0;
        /* If input is 15ms, don't do buffering(used only in 7.5ms case) */
        if (Length == AEC_WB_FRAME_SIZE) {
            gEcIn_1st_prc = false;
            gEcInBufIdx = false;
        }

    if(gEcIn_1st_prc  == true) {
        memset(AEC_IN_BUF1, 0, Length);
        memset(AEC_IN_BUF2, 0, Length);
        memset(AEC_IN_BUF3, 0, Length);
        memset(AEC_IN_BUFec, 0, Length);
        gEcIn_1st_prc = false;
    }
    memcpy((AEC_IN_BUF1 + gEcInBufIdx *Length), (U8*)InBufLeft, Length); //DNN_IN buffering control
    memcpy((AEC_IN_BUF2 + gEcInBufIdx *Length), (U8*)InBufRight, Length); //DNN_IN buffering control
    if(Channel_num <= 3) {
        memcpy((AEC_IN_BUF3 + gEcInBufIdx *Length), (U8*)InBufRight, Length); //DNN_IN buffering control
    } else {
        #ifdef ENABLE_2A2D_TEST
        memcpy((AEC_IN_BUF3 + gEcInBufIdx *Length), (U8*)InBufMic3, Length); //DNN_IN buffering control
        #endif
    }
    memcpy((AEC_IN_BUFec + gEcInBufIdx *Length), (U8*)EC_path_buf, Length); //DNN_IN buffering control
    if (Length != AEC_WB_FRAME_SIZE) {
        gEcInBufIdx  = (!gEcInBufIdx);
    }
#else
    U8* AEC_IN_BUF1 = (U8*)InBufLeft;
    U8* AEC_IN_BUF2 = (U8*)InBufRight;
    U8* AEC_IN_BUF3;
    if(Channel_num <= 3) {
        AEC_IN_BUF3 = (U8*)InBufRight;
    } else {
        #ifdef ENABLE_2A2D_TEST
        AEC_IN_BUF3 = (U8*)InBufMic3;
        #endif
    }
    U8* AEC_IN_BUFec = (U8*)EC_path_buf;
#endif
/// For 7.5ms AFE buffering ////////////////////////////////////////////////////////////////

#ifdef MTK_3RD_PARTY_NR
    S16 EC_out1_buf[DSP_AEC_OUT_BUF];
    S16 EC_out2_buf[DSP_AEC_OUT_BUF];
    S16 NR_out_buf[DSP_AEC_OUT_BUF];

    #ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
    if(gEcInBufIdx  == false) {
    #endif

    #if defined(AIR_IGO_NR_ENABLE)
        void *tx_nr_ptr;
        uint32_t *nvkey_nr_para;
        tx_nr_ptr = stream_function_get_working_buffer(para);
        if (g_igo_nr_init_flag == true) {
            nvkey_nr_para = (uint32_t *)p_reserved_NvKey;
            if (gDspAlgParameter.EscoMode.Rx == CVSD) {
                IGO_NR_Init(tx_nr_ptr, (uint32_t *)nvkey_nr_para, p_txeq_NvKey, _NB);
            } else {
                IGO_NR_Init(tx_nr_ptr, (uint32_t *)nvkey_nr_para, p_txeq_NvKey, _WB);
            }
            g_igo_nr_init_flag = false;
            DSP_MW_LOG_I("iGO NR init done", 0);
        }
        Voice_EC_Prcs((S16 *)AEC_IN_BUF1, (S16 *)AEC_IN_BUF2, (S16 *)AEC_IN_BUFec, EC_out1_buf, EC_out2_buf);
        IGO_NR_Prcs(EC_out1_buf, NR_out_buf, tx_nr_ptr);
    #elif defined(AIR_IGO_NR_PLUS_1_ENABLE)
        void *tx_nr_ptr;
        U8 EC_fb_buf[DSP_AEC_OUT_BUF];
        _SWITCH_MODE igo_mode;
        uint32_t *nvkey_nr_para;
        //IGO_1MIC_NR_PARAMS nr_params;
        if(Channel_num == 3) {
            AEC_IN_BUF2 = NULL;
        }
        //DSP_MW_LOG_I("iGO NR plus 1 Voice_EC_Inear_Prcs() %d, 0x%x, 0x%x, 0x%x, 0x%x", 5, Channel_num, InBufLeft, InBufRight, InBufMic3, EC_path_buf);
        Voice_EC_Inear_Prcs((S16 *)AEC_IN_BUF1, (S16 *)AEC_IN_BUF2, (S16 *)AEC_IN_BUF3, (S16 *)AEC_IN_BUFec, (S16 *)EC_out1_buf, (S16 *)EC_out2_buf, (S16 *)EC_fb_buf);
        #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
            LOG_AUDIO_DUMP((U8*)EC_out1_buf, AEC_WB_FRAME_SIZE, AUDIO_ENHANCEMENT_IN_L);
            LOG_AUDIO_DUMP((U8*)EC_fb_buf, AEC_WB_FRAME_SIZE, AUDIO_ENHANCEMENT_IN_R);
        #endif
        tx_nr_ptr = stream_function_get_working_buffer(para);
        igo_mode = IGO_NR_Get_Curr_Mode();
        if ((g_igo_nr_init_flag == true) || (igo_mode != g_iGO_NR_Curr_Mode)) {
            //nr_params.nr_strength = 7;
            nvkey_nr_para = (uint32_t *)p_reserved_NvKey;
            //DSP_MW_LOG_I("iGO NR plus 1 0x%x, 0x%x, 0x%x, 0x%x", 4, tx_nr_ptr, &nr_params, (uint32_t *)nvkey_nr_para, (uint8_t)igo_mode);
            //IGO_NR_Init(tx_nr_ptr, &nr_params, (uint32_t *)nvkey_nr_para, (uint8_t)igo_mode);
            DSP_MW_LOG_I("iGO NR plus 1 0x%x, 0x%x, 0x%x", 3, tx_nr_ptr, (uint32_t *)nvkey_nr_para, (uint8_t)igo_mode);
            //IGO_NR_Init(tx_nr_ptr, (uint32_t *)nvkey_nr_para, (uint8_t)igo_mode);
            if (gDspAlgParameter.EscoMode.Rx == CVSD) {
                IGO_NR_Init(tx_nr_ptr, (uint32_t *)nvkey_nr_para, (uint8_t)igo_mode, p_txeq_NvKey, p_txeq_NvKey, p_AstEq_NvKey, 0);
            } else {
                IGO_NR_Init(tx_nr_ptr, (uint32_t *)nvkey_nr_para, (uint8_t)igo_mode, p_txeq_NvKey, p_txeq_NvKey, p_AstEq_NvKey, 1);
            }
            g_igo_nr_init_flag = false;
            g_iGO_NR_Curr_Mode = igo_mode;
            DSP_MW_LOG_I("iGO NR plus 1 change mode to %d", 1, g_iGO_NR_Curr_Mode);
        }
        //DSP_MW_LOG_I("iGO NR plus 1 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", 5, (S16 *)EC_out1_buf, (S16 *)EC_out2_buf, (S16 *)EC_fb_buf, (S16 *)NR_out_buf, tx_nr_ptr);
        IGO_NR_Prcs((S16 *)EC_out1_buf, (S16 *)EC_fb_buf, (S16 *)EC_path_buf, (S16 *)NR_out_buf, tx_nr_ptr);
        #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
            LOG_AUDIO_DUMP((U8*)NR_out_buf, AEC_WB_FRAME_SIZE, AUDIO_ENHANCEMENT_OUT_L);
        #endif
    #else /*AIR_IGO_NR_ENABLE, AIR_IGO_NR_PLUS_1_ENABLE*/
        Voice_EC_Prcs((S16 *)AEC_IN_BUF1, (S16 *)AEC_IN_BUF2, (S16 *)AEC_IN_BUFec, EC_out1_buf, EC_out2_buf);
        //Voice_NR_Prcs(S16* EC_out1, S16* NR_out, S16 NR_gain); /* 3rd party customized NR process function */
        memcpy(NR_out_buf, EC_out1_buf, AEC_WB_FRAME_SIZE);
    #endif /*AIR_IGO_NR_ENABLE, AIR_IGO_NR_PLUS_1_ENABLE*/

    Voice_PostEC_Prcs((S16 *)NR_out_buf, OutBuf, &gDspAlgParameter.AecNr);
    #ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
        ECNR_OUT_idx = 0;
    }else {
        ECNR_OUT_idx = 1;
    }
    #endif
#else /*#ifdef MTK_3RD_PARTY_NR*/
    #ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
        if(gEcInBufIdx  == false) {
    #endif

    #ifdef MTK_INEAR_ENHANCEMENT
        Voice_WB_TX_Inear_Prcs(InBufLeft, InBufRight, EC_path_buf, OutBuf, &gDspAlgParameter.AecNr, &g_f_wind, &mic_status_o);
        if (AecMemoryPtr->prev_g_f_wind != g_f_wind) {
            DSP_MW_LOG_I("Prev g_f_wind = %d, curr g_f_wind = %d",2, AecMemoryPtr->prev_g_f_wind, g_f_wind);
            AecMemoryPtr->prev_g_f_wind = g_f_wind;
        }
        if (AecMemoryPtr->prev_mic_status_o != mic_status_o) {
            DSP_MW_LOG_I("Prev mic_status_o = %d, curr mic_status_o = %d",2, AecMemoryPtr->prev_mic_status_o, mic_status_o);
            AecMemoryPtr->prev_mic_status_o = mic_status_o;
        }
    #elif defined(MTK_DUALMIC_INEAR) && defined(ENABLE_2A2D_TEST)
        Voice_WB_TX_Inear_Prcs_V2((S16 *)AEC_IN_BUF1, (S16 *)AEC_IN_BUF2, (S16 *)AEC_IN_BUF3, (S16 *)AEC_IN_BUFec, OutBuf, &gDspAlgParameter.AecNr);
    #else
        Voice_WB_TX_Prcs((S16 *)AEC_IN_BUF1, (S16 *)AEC_IN_BUF2, (S16 *)AEC_IN_BUFec, OutBuf, &gDspAlgParameter.AecNr);
    #endif
    gDspAlgParameter.ECNR_Pitch_Last = get_txnr_pitch_last();
    #ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
        ECNR_OUT_idx = 0;
    }else {
        ECNR_OUT_idx = 1;
    }
    #endif
#endif /*#ifdef MTK_3RD_PARTY_NR*/
    #if defined(AIR_ULL_VOICE_LOW_LATENCY_ENABLE)
         memcpy((U8*)InBufLeft, (U8*)OutBuf+ ECNR_OUT_idx*(U8)Length, Length);
    #else
         memcpy(InBufLeft, OutBuf, Length);
    #endif

    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
        LOG_AUDIO_DUMP((U8*)InBufLeft, Length, VOICE_TX_NR_OUT);
    #endif

    i2PostAvcVol = gDspAlgParameter.AecNr.avc_vol;
    //DSP_MW_LOG_I("[NDVC][DSP] pre_avc_vol: %d, post_avc_vol: %d",2, i2PreAvcVol,i2PostAvcVol);
    if(i2PostAvcVol>>6 != i2PreAvcVol>>6){
        if(avc_vol_compare(i2PreAvcVol, i2PostAvcVol)){
            if((gfgAvcUpdate == false) && (gfgAvcSend == true)){
                DSP_MW_LOG_I("[NDVC][DSP send]former:%d, later:%d",2, i2PreAvcVol, i2PostAvcVol);
                memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
                msg.ccni_message[0] = MSG_DSP2MCU_AVC_PARA_SEND << 16;
                msg.ccni_message[1] = i2PostAvcVol;
                aud_msg_tx_handler(msg, 0, false);
                gfgAvcSend = false;
            }
        }
    }
    gDspAlgParameter.AecNr.avc_vol = i2PreAvcVol;

    if(gfgAvcUpdate == true){
       //DSP_MW_LOG_I("[NDVC][DSP update] gDspAvcVol: %d, gfgAvcUpdate: %d , gfgAvcSend: %d\r\n",2, gi2AvcVol, gfgAvcUpdate, gfgAvcSend);
       gDspAlgParameter.AecNr.avc_vol = gi2AvcVol;
       gfgAvcUpdate = false;
       gfgAvcSend = true;
    }

    gDspAlgParameter.AecReady = TRUE;

    return FALSE;
}


/**
 * stream_function_nr_process
 *
 * The main process for TX AEC/NR
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_nr_process (void *para)
{
    S16* Buf    = (S16*)stream_function_get_1st_inout_buffer(para);
    U16 Length  = (U16)stream_function_get_output_size(para);

    if (AEC_NR_MemCheck() == FALSE)
    {
        //configASSERT(0);
        stream_feature_reinitialize(para);
        DSP_MW_LOG_I("[NR] reinitialize", 0);
        return FALSE;
    }
    gNRnum++;

    if ((AecMemoryPtr->NvKey.AecNr.ENABLE == 0) || (gAecNrEn_MP == FALSE))
    {
        DSP_MW_LOG_I("[NR] Disable, gNRnum:%d", 1, gNRnum);
        return FALSE;
    }

#ifndef MTK_3RD_PARTY_NR
    if (gNeedChangeRxEQ) {
        EQ_update(p_ecnr_bss, p_rxeq_NvKey);
        gNeedChangeRxEQ = false;
        DSP_MW_LOG_I("aec_nr change RX EQ param", 0);
    }
#endif

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)Buf, (U32)Length, VOICE_RX_NR_IN);
#endif

    switch (gDspAlgParameter.EscoMode.Rx)
    {
        case CVSD:
            /*#if (!AEC_FORCE_WB_PROCEDURE)
            //Fix CVSD decoder out 16kHz
            if (Length != AEC_NB_FRAME_SIZE)
            {
                return TRUE;
            }
            Voice_NB_RX_Prcs(Buf);
            #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                 LOG_AUDIO_DUMP((U8*)Buf, (U32)Length, VOICE_RX_NR_OUT);
            #endif
            break;
            #endif*/
        case mSBC:
            if (Length != AEC_WB_FRAME_SIZE)
            {
                return (Length != 0);
            }
            Voice_WB_RX_Prcs(Buf);
            #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                LOG_AUDIO_DUMP((U8*)Buf, (U32)Length, VOICE_RX_NR_OUT);
            #endif
            break;

        default:
            return TRUE;
    }

    U16 i;
    for (i = 1 ; i < stream_function_get_device_channel_number(para) ; i++)
    {
        memcpy(stream_function_get_inout_buffer(para, i+1),
               stream_function_get_inout_buffer(para, 1),
               stream_function_get_output_size(para));
    }

    gDspAlgParameter.AecReady = TRUE;

    return FALSE;
}



/**
 * AEC_NR_NVKEY_Init
 *
 * The NVKEY init process for AEC/NR
 *
 *
 * @mode : 0 for NB, 1 for WB
 *
 */
BOOL AEC_NR_NVKEY_Init (SCO_CODEC mode)
{
    U16 RxEqKeyId, TxEqKeyId, RxEqKeyId_forANC = 0;

    switch (mode)
    {
        case CVSD:
            RxEqKeyId = NVKEY_DSP_PARA_NB_RX_EQ;
            TxEqKeyId = NVKEY_DSP_PARA_NB_TX_EQ;
            #ifdef MTK_ANC_ENABLE
            #ifndef MTK_ANC_V2
            RxEqKeyId_forANC = NVKEY_DSP_PARA_NB_RX_EQ_2ND;
            #endif
            #endif
            break;

        case mSBC:
            RxEqKeyId = NVKEY_DSP_PARA_WB_RX_EQ;
            TxEqKeyId = NVKEY_DSP_PARA_WB_TX_EQ;
            #ifdef MTK_ANC_ENABLE
            #ifndef MTK_ANC_V2
            RxEqKeyId_forANC = NVKEY_DSP_PARA_WB_RX_EQ_2ND;
            #endif
            #endif
            break;

        default:
            return FALSE;
    }


    #if (AEC_TEST_DEFAULT_PARA)
    memcpy(&AecMemoryPtr->NvKey.AecNr,&ECNR_DspParaInit,sizeof(DSP_PARA_AEC_NR_STRU));
    memcpy(&AecMemoryPtr->NvKey.RxEq,&WBtxEQ_DspParaInit,sizeof(DSP_PARA_WB_RX_EQ_STRU));
    memcpy(&AecMemoryPtr->NvKey.TxEq,&WBrxEQ_DspParaInit,sizeof(DSP_PARA_WB_TX_EQ_STRU));
    #else
    nvkey_read_full_key(NVKEY_DSP_PARA_AEC_NR,p_ecnr_NvKey, sizeof(DSP_PARA_AEC_NR_STRU));
    nvkey_read_full_key(TxEqKeyId,p_txeq_NvKey, ((mode==mSBC) || AEC_FORCE_WB_PROCEDURE)?sizeof(DSP_PARA_WB_TX_EQ_STRU):sizeof(DSP_PARA_NB_TX_EQ_STRU));
    aec_nr_get_rx_eq_nvkey(RxEqKeyId, RxEqKeyId_forANC, p_rxeq_NvKey, ((mode==mSBC) || AEC_FORCE_WB_PROCEDURE)?sizeof(DSP_PARA_WB_RX_EQ_STRU):sizeof(DSP_PARA_NB_RX_EQ_STRU));
    #if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
    nvkey_read_full_key(NVKEY_DSP_PARA_INEAR_EQ, p_InEarEq_NvKey, sizeof(DSP_PARA_InEarEQ_STRU));
    #endif
    #if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    nvkey_read_full_key(NVKEY_DSP_PARA_AST_EQ, p_AstEq_NvKey, sizeof(DSP_PARA_AST_EQ_STRU));
    #endif
    #ifdef MTK_3RD_PARTY_NR
    nvkey_read_full_key(NVKEY_DSP_PARA_RESERVED, p_reserved_NvKey, sizeof(DSP_PARA_RESERVED_STRU));
    #endif

    if(mode==CVSD)
    {
        uint16_t *pu16rx = (uint16_t *)(p_rxeq_NvKey+sizeof(DSP_PARA_NB_RX_EQ_STRU));
        uint16_t *pu16tx = (uint16_t *)(p_txeq_NvKey+sizeof(DSP_PARA_NB_RX_EQ_STRU));
        uint32_t i;
        for(i = 0; i < ((sizeof(DSP_PARA_WB_RX_EQ_STRU)-sizeof(DSP_PARA_NB_RX_EQ_STRU))>>1); i++)
        {
            *pu16rx = 0x1000; pu16rx++;
            *pu16tx = 0x1000; pu16tx++;
        }
    }
    #endif

    return TRUE;
}


/**
 * AEC_NR_GetRefGain
 *
 * The get current reference gain for AEC/NR
 *
 *
 * @mode : 0 for NB, 1 for WB
 *
 */
VOID AEC_NR_GetRefGain (S16* GainAddr)
{
    uint32_t i;

    configASSERT(GainAddr != NULL);

    if ((AecMemoryPtr != NULL) && (gDspAlgParameter.AecReady != FALSE))
    {
        if (AEC_VALID_MEMORY_CHECK_VALUE == AecMemoryPtr->MemoryCheck)
        {
            /*
             * airoha 1/2 MIC: EC_REF_GAIN_READBACK (16 bytes, for 1MIC case, 0 for gain[4]~gain[7])
             * airoha 1+1 MIC: EC_REF_GAIN_READBACK (16 bytes, 0 for gain[4]~gain[7]), IE_EC_REF_GAIN_READBACK(8 bytes for +1)
             * airoha 2+1 MIC: EC_REF_GAIN_READBACK (16 bytes), IE_EC_REF_GAIN_READBACK(8 bytes for +1)
             * 3rd 1/2 MIC: EC_REF_GAIN_READBACK (16 bytes, for 1MIC case, 0 for gain[4]~gain[7])
             * 3rd 1+1 MIC or 3rd 2+1 MIC: EC_REF_GAIN_READBACK (16 bytes, for 1MIC case, 0 for gain[4]~gain[7])
             *                             EC_Inear_REF_GAIN_READBACK(8 bytes for +1)
             */
            if (g_ecnr_prev_process_enable == false) {
                EC_REF_GAIN_READBACK(GainAddr);
            }
#ifdef AIR_ECNR_PREV_PART_ENABLE
            else {
                EC120_REF_GAIN_READBACK(GainAddr);
            }
#endif
            for (i = 0; i < 8; i++) {
                DSP_MW_LOG_I("DSP get ref gain, GainAddr:0x%x", 1, GainAddr[i]);
            }
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
            IE_EC_REF_GAIN_READBACK(GainAddr + AEC_REF_GAIN_SIZE);
            for (i = 0; i < 4; i++) {
                DSP_MW_LOG_I("DSP get inear ref gain, GainAddr:0x%x", 1, GainAddr[AEC_REF_GAIN_SIZE + i]);
            }
#endif
#if defined(MTK_3RD_PARTY_NR) && defined(AIR_IGO_NR_PLUS_1_ENABLE)
            EC_Inear_REF_GAIN_READBACK(GainAddr + AEC_REF_GAIN_SIZE);
            for (i = 0; i < 4; i++) {
                DSP_MW_LOG_I("DSP get inear ref gain, GainAddr:0x%x", 1, GainAddr[AEC_REF_GAIN_SIZE + i]);
            }
#endif
            return;
        }
    }


#if defined(AIR_EC120_ENABLE)
    if ((Ec120MemoryPtr != NULL) && (gDspAlgParameter.AecReady != FALSE))
    {
        if (EC120_VALID_MEMORY_CHECK_VALUE == Ec120MemoryPtr->MemoryCheck)
        {
            EC120_REF_GAIN_READBACK(GainAddr);
            for (i = 0; i < 8; i++) {
                DSP_MW_LOG_I("[EC120] DSP get ref gain, GainAddr:0x%x", 1, GainAddr[i]);
            }
            return;
        }
    }
#endif


    memset(GainAddr, 0, sizeof(S16)*8);
}

uint32_t AEC_NR_Query_Lib_Version(dsp_alg_lib_type_t type, uint8_t *version)
{
    uint32_t length;

    if (AEC_NR_Check_Runing_Status() == false) {
        DSP_MW_LOG_I("[DSP][AEC/NR] is not in working status", 0);
        return 0;
    }

    if (type == DSP_ALG_LIB_TYPE_IGO) {
#if defined(AIR_IGO_NR_PLUS_1_ENABLE) || defined(AIR_IGO_NR_ENABLE)
        IGO_NR_Get_Lib_Version((version_info_t *)version);
        length = sizeof(version_info_t);
#else
        length = 0;
#endif
    } else {
        length = 0;
    }

    return length;
}

/**
 * AEC_NR_Check2MicEnabled
 *
 * API to check if 2-mic ECNR algorithm is enabled
 *
 * @Status : TRUE-> 2-mic / False-> 1-mic
 *
 */
bool AEC_NR_Check2MicEnabled(void)
{
    bool Status;
    if ((AecMemoryPtr->NvKey.AecNr.AEC_NR_EN & 0x0005) == 0x0005)
    {
        //DSP_MW_LOG_I("[ECNR] It's 2-mic ECNR algorithm, 0x%x", 1, AecMemoryPtr->NvKey.AecNr.AEC_NR_EN);
        Status = TRUE;
    }
    else
    {
        //DSP_MW_LOG_I("[ECNR] It's 1-mic ECNR algorithm, 0x%x", 1, AecMemoryPtr->NvKey.AecNr.AEC_NR_EN);
        Status = FALSE;
    }

    return Status;
}

#ifdef MTK_AIRDUMP_EN
/**
 * AEC_NR_DumpEnable
 *
 * API for enable/disable air dump
 *
 * @state  : True for enable, false for disable
 *
 */
void AEC_NR_AirDumpEnable(bool state)
{
    rAirDumpDSPCtrl.airdump_en = state;

    if(state == TRUE){
        rAirDumpCtrl->length = (sizeof(S16))*(AEC_AIRDUMP_FRAME_SIZE)*AEC_AIRDUMP_FRAME_NUM;
        rAirDumpCtrl->write_offset= 0;
        rAirDumpCtrl->read_offset= 0;
        rAirDumpCtrl->notify_count= 0;
        rAirDumpDSPCtrl.wo_state.over_halfbuf = false;
        DSP_MW_LOG_I("[AirDump][DSP] AIRDUMP Enable Done\r\n", 0);
    } else if(state == FALSE) {
        rAirDumpCtrl->length = 0;
        rAirDumpCtrl->write_offset= 0;
        rAirDumpCtrl->read_offset= 0;
        rAirDumpCtrl->notify_count= 0;
        rAirDumpDSPCtrl.wo_state.over_halfbuf = false;
        DSP_MW_LOG_I("[AirDump][DSP] AIRDUMP Disable Done\r\n", 0);
    }
}

/**
 * AEC_NR_AirDump
 *
 * API for AirDump (PreLim Coefficient and Reference Gain)
 *
 */
void AEC_NR_AirDump (void)
{
    S16 i2Buf[AEC_AIRDUMP_FRAME_SIZE];
    U16 u2BufLen = sizeof(S16)*(AEC_AIRDUMP_FRAME_SIZE);
    U8* wptr;
    U32 writeoffset;

    EC_PreLim_Coef_READBACK(&i2Buf[0]);
    EC_REF_GAIN_READBACK(&i2Buf[AEC_PRELIM_COEF_SIZE]);

    i2Buf[AEC_PRELIM_COEF_SIZE+AEC_REF_GAIN_SIZE] = DaulMIC_power_Coef_READBACK();
    
#ifdef MTK_INEAR_ENHANCEMENT
    Get_AirDump_Inear(&i2Buf[AEC_PRELIM_COEF_SIZE+AEC_REF_GAIN_SIZE+AEC_DUALMIC_POWER_COEF_SIZE]);
#endif

    wptr = (U8*)(&rAirDumpCtrl->data[0] + rAirDumpCtrl->write_offset);
    memcpy(wptr, (U8*)i2Buf, u2BufLen);
    writeoffset = (rAirDumpCtrl->write_offset + u2BufLen)%(rAirDumpCtrl->length);
    //DSP_MW_LOG_I("[AirDump][DSP] old wo:%d, update wo:%d, totalbuflen:%d",3, rAirDumpCtrl->write_offset, writeoffset, rAirDumpCtrl->length);
    AirDump_update_notify_count(writeoffset);
    AirDump_update_writeoffset_share_information(writeoffset);
}


void AirDump_update_notify_count(U32 writeoffset)
{
    bool wo_past = rAirDumpDSPCtrl.wo_state.past_state;
    bool wo_over = rAirDumpDSPCtrl.wo_state.over_halfbuf;
    if((writeoffset >= (rAirDumpCtrl->length/2)) && (wo_over == FALSE)){
        wo_over = TRUE;
    } else if ((writeoffset < (rAirDumpCtrl->length/2)) && (wo_over == TRUE)) {
        wo_over = FALSE;
    }

    if(wo_past != wo_over)
    {
        rAirDumpCtrl->notify_count++;
        DSP_MW_LOG_I("[AirDump][DSP] buffer cnt: %d, wo:%d, wo_past:%d, wo_over:%d",4, rAirDumpCtrl->notify_count, writeoffset, wo_past, wo_over);
        wo_past = wo_over;
        rAirDumpDSPCtrl.wo_state.over_halfbuf = wo_over;
        rAirDumpDSPCtrl.wo_state.past_state = wo_past;
    }
}

void AirDump_update_writeoffset_share_information(U32 writeoffset)
{
    rAirDumpCtrl->write_offset = writeoffset;
}
#endif

VOID aec_nr_get_rx_eq_nvkey(U16 nvkey_0, U16 nvkey_1, VOID *p_nvkey_dst, U32 length)
{
    mem_nvdm_info_t dsp0_nvdm;
    sysram_status_t status;
    dsp0_nvdm.nvdm_id   = nvkey_0;
    dsp0_nvdm.length    = length;
    dsp0_nvdm.mem_pt    = p_nvkey_dst;
    status = nat_table_read_audio_nvdm_data(dsp0_nvdm, c_sram_mode);
    #ifdef MTK_ANC_ENABLE
    if (status == NVDM_STATUS_NAT_ITEM_NOT_FOUND) {
        dsp0_nvdm.nvdm_id = nvkey_1;
        status = nat_table_read_audio_nvdm_data(dsp0_nvdm, c_sram_mode);
        DSP_MW_LOG_W("aec_nr rx eq nvkey: 0x%x", 1, nvkey_1);
    }
    #else
    UNUSED(nvkey_1);
    #endif
    if (status != NVDM_STATUS_NAT_OK) {
        DSP_MW_LOG_E("aec_nr_get_rx_eq_nvkey read nvkey error, nvkey_id:0x%x status:%d", 2, dsp0_nvdm.nvdm_id, status);
        platform_assert("NvKey CheckSum or ITEM_NOT_FOUND Error", __FILE__, __LINE__);
    }
}

VOID aec_nr_set_param(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    aec_nr_param_type_t msg16 = (aec_nr_param_type_t)(msg.ccni_message[0] & 0xFFFF);
    U16 RxEqKeyId, RxEqKeyId_forANC = 0;
    SCO_CODEC sco_type = gDspAlgParameter.EscoMode.Rx;
    UNUSED(ack);

    switch(sco_type)
    {
        case CVSD:
            RxEqKeyId = NVKEY_DSP_PARA_NB_RX_EQ;
            #ifdef MTK_ANC_ENABLE
            #ifndef MTK_ANC_V2
            RxEqKeyId_forANC = NVKEY_DSP_PARA_NB_RX_EQ_2ND;
            #endif
            #endif
            break;
        case mSBC:
            RxEqKeyId = NVKEY_DSP_PARA_WB_RX_EQ;
            #ifdef MTK_ANC_ENABLE
            #ifndef MTK_ANC_V2
            RxEqKeyId_forANC = NVKEY_DSP_PARA_WB_RX_EQ_2ND;
            #endif
            #endif
            break;
        default:
            return;
    }

    switch (msg16)
    {
        case AEC_NR_PARAM_TYPE_RX_EQ:
            if (AEC_NR_MemCheck() && p_rxeq_NvKey) {
                aec_nr_get_rx_eq_nvkey(RxEqKeyId, RxEqKeyId_forANC, p_rxeq_NvKey, ((sco_type==mSBC) || AEC_FORCE_WB_PROCEDURE)?sizeof(DSP_PARA_WB_RX_EQ_STRU):sizeof(DSP_PARA_NB_RX_EQ_STRU));
                if(sco_type==CVSD)
                {
                    uint16_t *pu16rx = (uint16_t *)(p_rxeq_NvKey+sizeof(DSP_PARA_NB_RX_EQ_STRU));
                    uint32_t i;
                    for(i = 0; i < ((sizeof(DSP_PARA_WB_RX_EQ_STRU)-sizeof(DSP_PARA_NB_RX_EQ_STRU))>>1); i++)
                    {
                        *pu16rx = 0x1000; pu16rx++;
                    }
                }
                gNeedChangeRxEQ = true;
            }
            break;
        default:
            return;
    }
    DSP_MW_LOG_I("aec_nr_set_param type: %d finish", 1, msg16);
}

#ifdef PRELOADER_ENABLE
BOOL AEC_NR_Open  (VOID* para)
{
    DSP_MW_LOG_I("[PIC] AEC/NR Open", 0);
    UNUSED(para);
    return TRUE;
}

BOOL AEC_NR_Close (VOID* para)
{
    DSP_MW_LOG_I("[PIC] AEC/NR Close", 0);
    UNUSED(para);
    return TRUE;
}

bool avc_vol_compare(S16 i2PreAvcVol, S16 i2PostAvcVol)
{
    bool diffrent_dB_class;
    if(avc_vol_dB0<=i2PreAvcVol && i2PreAvcVol<avc_vol_dB4)
    {
        if(avc_vol_dB0<=i2PostAvcVol && i2PostAvcVol<avc_vol_dB4)
        {
            if(i2PostAvcVol == avc_vol_dB0 && i2PreAvcVol != avc_vol_dB0){
                return diffrent_dB_class = true;
            }
           return diffrent_dB_class = false;
        }
        else
        {
           return diffrent_dB_class = true;
        }

    }
    else if(avc_vol_dB4<=i2PreAvcVol && i2PreAvcVol<avc_vol_dB8)
    {
        if(avc_vol_dB4<=i2PostAvcVol && i2PostAvcVol<avc_vol_dB8)
        {
           return diffrent_dB_class = false;
        }
        else
        {
           return diffrent_dB_class = true;
        }
    }
    else if(avc_vol_dB8<=i2PreAvcVol)
    {
        if(avc_vol_dB8<=i2PostAvcVol)
        {
            if(i2PostAvcVol == avc_vol_max && i2PreAvcVol != avc_vol_max){
                return diffrent_dB_class = true;
            }
           return diffrent_dB_class = false;
        }
        else
        {
           return diffrent_dB_class = true;
        }
    }
    else
    {
        return diffrent_dB_class = false;
    }

}


#if defined(AIR_EC120_ENABLE)

/**
 * EC_120_MemCheck
 *
 * This function is used to check init memory space for AEC/NR process
 *
 *
 * @para : Default parameter of callback function
 * @return : Check result
 */
bool EC_120_MemCheck (void)
{
    if (NULL != Ec120MemoryPtr)
    {
        if (EC120_VALID_MEMORY_CHECK_VALUE == Ec120MemoryPtr->MemoryCheck)
        {
            return TRUE;
        }
    }
    return FALSE;
}


/**
 * EC_120_MemInit
 *
 * This function is used to init memory space for EC120 process
 *
 *
 * @para : Default parameter of callback function
 * @return : Initialize result
 */
bool EC_120_MemInit (void* para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    if (!AEC_NR_MemCheck())
    {
        if (!(Ec120MemoryPtr && (Ec120MemoryPtr->MemoryCheck == EC120_RUNTIME_INVALID_MEMORY_CHECK_VALUE))) {
            Ec120MemoryPtr = (EC120_INSTANCE_PTR)DSPMEM_tmalloc(DAV_TASK_ID, DSP_EC_120_MEMSIZE+get_ec120_memsize(), stream_ptr);
        }
        Ec120MemoryPtr->MemoryCheck = EC120_VALID_MEMORY_CHECK_VALUE;

        gDspAlgParameter.AecReady = FALSE;
        return FALSE;
    }
    return TRUE;
}



/**
 * stream_function_ec120_initialize
 *
 * This function is used to init memory space for EC120 process
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_function_ec_120_initialize (void *para)
{
    SCO_CODEC     sco_type;
    DSP_MW_LOG_I("stream_function_ec_120_initialize start, EC_LIB VER:%d", 1, get_EC120_SVN());

    if (EC_120_MemInit(para))
    {
        return FALSE;
    }

    p_ecnr_bss      =  &Ec120MemoryPtr->ScratchMemory[0];
    p_ecnr_NvKey    =  &Ec120MemoryPtr->NvKey.AecNr;
    p_rxeq_NvKey    =  &Ec120MemoryPtr->NvKey.RxEq;
    p_txeq_NvKey    =  &Ec120MemoryPtr->NvKey.TxEq;
    #ifdef MTK_3RD_PARTY_NR
    p_reserved_NvKey = &Ec120MemoryPtr->NvKey.ResKey;
    #endif /*MTK_3RD_PARTY_NR*/
    #ifdef MTK_AIRDUMP_EN
    rAirDumpDSPCtrl.airdump_en = false;
    #endif
    gDnnIn_1st_prc = true;
    gDnnInBufIdx = true;

    sco_type = gDspAlgParameter.EscoMode.Rx;
    switch (sco_type)
    {
        case CVSD:
            //Fix CVSD decoder out 16kHz
            AEC_NR_NVKEY_Init(CVSD);
            Voice_EC120_Init(1, p_ecnr_bss, p_ecnr_NvKey, (S16 *)&Ec120MemoryPtr->NvKey.AecNr.WB_NR_TX_NOISE_GAIN_LIMITER);
            break;


        case mSBC:
            AEC_NR_NVKEY_Init(mSBC);
            Voice_EC120_Init(0, p_ecnr_bss, p_ecnr_NvKey, (S16 *)&Ec120MemoryPtr->NvKey.AecNr.WB_NR_TX_NOISE_GAIN_LIMITER);
            break;

        default:
            return TRUE;
    }

#ifdef MTK_3RD_PARTY_NR
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    g_igo_nr_init_flag = true;
#endif
#endif

    DSP_MW_LOG_I("stream_function_ec_120_initialize done",0);

    return FALSE;
}

/**
 * stream_function_ec120_process
 *
 * The main process for TX EC120
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_ec120_process (void *para)
{
    U8   Channel_num    = stream_function_get_channel_number(para);
    S16* InBufLeft      = (S16*)stream_function_get_1st_inout_buffer(para);
    S16* InBufRight     = (S16*)stream_function_get_2nd_inout_buffer(para);
    U16 Length          = (U16)stream_function_get_output_size(para);
    U8* DNN_IN          = (U8*)Ec120MemoryPtr->DNN_IN;
    U8* DNN_OUT         = (U8*)Ec120MemoryPtr->DNN_OUT;
    S16* OutBuf         = (S16*)Ec120MemoryPtr->AecOut;
    S16* EC_path_buf    = (Channel_num == 2) ? (S16*)stream_function_get_2nd_inout_buffer(para) : (S16*)stream_function_get_3rd_inout_buffer(para);
    U8 DNN_OUT_idx = 0;
    S16 EC_out1_buf[Length];
    S16 EC_out2_buf[Length];

    if (EC_120_MemCheck() == FALSE)
    {
        stream_feature_reinitialize(para);
		DSP_MW_LOG_I("[stream_function_ec120_process] reinitialize", 0);
        return FALSE;
    }

    if ((Ec120MemoryPtr->NvKey.AecNr.ENABLE == 0) || (gAecNrEn_MP == FALSE))
    {
        //DSP_MW_LOG_I("[AEC] Disable", 0);
        return FALSE;
    }

    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)EC_path_buf, (U32)Length, VOICE_TX_REF);
    LOG_AUDIO_DUMP((U8*)InBufLeft, (U32)Length, VOICE_TX_MIC_0);
    #endif

    #if defined(AIR_IGO_NR_ENABLE)
    void *tx_nr_ptr;
    uint32_t *nvkey_nr_para;
    tx_nr_ptr = stream_function_get_working_buffer(para);
    if (g_igo_nr_init_flag == true) {
        nvkey_nr_para = (uint32_t *)p_reserved_NvKey;
        if (gDspAlgParameter.EscoMode.Rx == CVSD) {
            IGO_NR_Init(tx_nr_ptr, (uint32_t *)nvkey_nr_para, p_txeq_NvKey, _NB);
        } else {
            IGO_NR_Init(tx_nr_ptr, (uint32_t *)nvkey_nr_para, p_txeq_NvKey, _WB);
        }
        g_igo_nr_init_flag = false;
        DSP_MW_LOG_I("iGO NR init done", 0);
    }
    #endif

    // PreEC (frame size unit: 7.5ms / 240 byte) ///////////////////////////////
    Voice_EC120_Prcs(InBufLeft, InBufRight, EC_path_buf, EC_out1_buf, EC_out2_buf); // Only support 1-mic EC
    if(gDnnIn_1st_prc == true) {
        memset(DNN_IN, 0, Length);
        gDnnIn_1st_prc = false;
    }
    memcpy((DNN_IN + gDnnInBufIdx*Length), (U8*)EC_out1_buf, Length); //DNN_IN buffering control
    gDnnInBufIdx = (!gDnnInBufIdx);

    // IGO_NR (frame size unit: 15ms / 480 byte) ///////////////////////////////
    #if defined(AIR_IGO_NR_ENABLE)
    if(gDnnInBufIdx == false) {
        #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
           LOG_AUDIO_DUMP(DNN_IN, Length<<1, AUDIO_ENHANCEMENT_IN_L);
        #endif
        IGO_NR_Prcs((S16 *)DNN_IN, (S16 *)DNN_OUT, tx_nr_ptr);
        #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
           LOG_AUDIO_DUMP((U8*)DNN_OUT, Length<<1, AUDIO_ENHANCEMENT_OUT_L);
        #endif
        DNN_OUT_idx = 0;
    } else {
        DNN_OUT_idx = 1;
    }
    #endif

    // PostEC (frame size unit: 7.5ms / 240 byte) //////////////////////////////
    Voice_PostEC120_Prcs((S16 *)(DNN_OUT + DNN_OUT_idx*Length), OutBuf, &gDspAlgParameter.AecNr);
    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
       LOG_AUDIO_DUMP((U8*)OutBuf, (U32)Length, VOICE_TX_NR_OUT);
    #endif

    // Copy outbuf to the main mic buf /////////////////////////////////////////
    memcpy(InBufLeft, OutBuf, Length);

    gDspAlgParameter.AecReady = TRUE;

    return FALSE;
}

#endif /*AIR_EC120_ENABLE*/


#endif


#if defined(MTK_3RD_PARTY_NR)
#if defined(AIR_ECNR_PREV_PART_ENABLE) && defined(AIR_EC120_ENABLE)

#if defined(AIR_IGO_NR_PLUS_1_ENABLE) || !defined(AIR_IGO_NR_ENABLE)
#error "Unsupported IGO feature option!!!"
#endif

uint8_t AEC_NR_Get_PostEC_Gain(void)
{
    return gDspAlgParameter.PostEC_Gain;
}

bool stream_function_ecnr_prev_initialize(void *para)
{
    uint32_t alloc_size;

#ifdef AIR_IGO_NR_ENABLE
    alloc_size = DSP_AEC_NR_MEMSIZE + get_ec120_memsize();
#endif
    if (AEC_NR_MemInit(para, alloc_size) && (!AecMemoryPtr->deinit_process)) {
        DSP_MW_LOG_I("[DSP][AEC/NR] bypass prev intialize", 0);
        return FALSE;
    }

    p_ecnr_bss      =  &AecMemoryPtr->ScratchMemory[0];
    p_ecnr_NvKey    =  &AecMemoryPtr->NvKey.AecNr;
    p_rxeq_NvKey    =  &AecMemoryPtr->NvKey.RxEq;
    p_txeq_NvKey    =  &AecMemoryPtr->NvKey.TxEq;
    p_reserved_NvKey = &AecMemoryPtr->NvKey.ResKey;
    AEC_NR_NVKEY_Init(mSBC);

#if defined(AIR_IGO_NR_ENABLE)
    Voice_EC120_Init(mSBC, p_ecnr_bss, p_ecnr_NvKey, (int16_t *)&AecMemoryPtr->NvKey.AecNr.WB_NR_TX_NOISE_GAIN_LIMITER);
#endif

    DSP_MW_LOG_I("[DSP][AEC/NR] 3rd party prev version %d", 1, get_EC120_SVN());

    return FALSE;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_ecnr_prev_process(void *para)
{
    uint8_t Channel_num;
    uint16_t Length;
    int16_t *InBufLeft, *InBufRight, *OutBuf, *EC_path_buf;
#ifdef ENABLE_2A2D_TEST
    int16_t *InBufMic3;
#endif
    uint8_t extra_OutBuf[AEC_75ms_FRAME_SIZE];

    if (AEC_NR_MemCheck() == FALSE) {
        stream_feature_reinitialize(para);
        DSP_MW_LOG_I("[DSP][AEC/NR] reinitialize the stream feature list", 0);
        return FALSE;
    }

    Length = (uint16_t)stream_function_get_output_size(para);
    if (Length != AEC_75ms_FRAME_SIZE) {
        return TRUE;
    }

    Channel_num = stream_function_get_channel_number(para);
    InBufLeft = (int16_t *)stream_function_get_1st_inout_buffer(para);
    switch (Channel_num) {
        case 2:
            InBufRight = NULL;
            InBufMic3 = NULL;
            EC_path_buf = (int16_t *)stream_function_get_2nd_inout_buffer(para);
            break;
        case 3:
            InBufRight = (int16_t *)stream_function_get_2nd_inout_buffer(para);
            InBufMic3 = NULL;
            EC_path_buf = (int16_t *)stream_function_get_3rd_inout_buffer(para);
            break;
#ifdef ENABLE_2A2D_TEST
        case 4:
            InBufRight = (int16_t *)stream_function_get_2nd_inout_buffer(para);
            InBufMic3 = (int16_t *)stream_function_get_3rd_inout_buffer(para);
            EC_path_buf = (int16_t *)stream_function_get_4th_inout_buffer(para);
            break;
        default:
            DSP_MW_LOG_E("[DSP][AEC/NR] MIC number un-support %d", 1, Channel_num);
            configASSERT(0);
#endif
    }

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    switch (Channel_num) {
        case 2:
            LOG_AUDIO_DUMP((U8*)InBufLeft, (U32)Length, VOICE_TX_MIC_0);
            break;
        case 3:
            LOG_AUDIO_DUMP((U8*)InBufLeft, (U32)Length, VOICE_TX_MIC_0);
            LOG_AUDIO_DUMP((U8*)InBufRight, (U32)Length, VOICE_TX_MIC_1);
            break;
#ifdef ENABLE_2A2D_TEST
        case 4:
            LOG_AUDIO_DUMP((U8*)InBufLeft, (U32)Length, VOICE_TX_MIC_0);
            LOG_AUDIO_DUMP((U8*)InBufRight, (U32)Length, VOICE_TX_MIC_1);
            LOG_AUDIO_DUMP((U8*)InBufMic3, (U32)Length, VOICE_TX_MIC_2);
            break;
#endif
    }
    LOG_AUDIO_DUMP((U8*)EC_path_buf, (U32)Length, VOICE_TX_REF);
#endif

    if ((AecMemoryPtr->NvKey.AecNr.ENABLE == 0) || (gAecNrEn_MP == FALSE))
    {
        //DSP_MW_LOG_I("[AEC] Disable", 0);
        return FALSE;
    }

    OutBuf = (int16_t *)(AecMemoryPtr->AecOut);
#if defined(AIR_IGO_NR_ENABLE)
    Voice_EC120_Prcs((int16_t *)InBufLeft, (int16_t *)InBufRight, (int16_t *)EC_path_buf, OutBuf, extra_OutBuf);
#endif
    memcpy(InBufLeft, OutBuf, Length);

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8 *)InBufLeft, Length, AUDIO_ENHANCEMENT_IN_L);
#endif

    /* Get the EC gain for post EC reference */
    gDspAlgParameter.PostEC_Gain = PostEC120_Info(&(gDspAlgParameter.AecNr));

    gDspAlgParameter.AecReady = TRUE;

    return FALSE;
}

#endif

#ifdef AIR_ECNR_POST_PART_ENABLE

#if defined(AIR_IGO_NR_PLUS_1_ENABLE) || !defined(AIR_IGO_NR_ENABLE)
#error "Unsupported IGO feature option!!!"
#endif

extern uint8_t AEC_NR_Port_Get_PostEC_Gain(void);
bool stream_function_ecnr_post_initialize(void *para)
{
    uint32_t alloc_size;
    void *tx_nr_ptr;

    alloc_size = DSP_AEC_NR_MEMSIZE + get_post_ec_memsize();
    if (AEC_NR_MemInit(para, alloc_size) && (!AecMemoryPtr->deinit_process)) {
        DSP_MW_LOG_I("[DSP][AEC/NR] bypass post intialize", 0);
        return FALSE;
    }

    p_ecnr_bss      =  &AecMemoryPtr->ScratchMemory[0];
    p_ecnr_NvKey    =  &AecMemoryPtr->NvKey.AecNr;
    p_rxeq_NvKey    =  &AecMemoryPtr->NvKey.RxEq;
    p_txeq_NvKey    =  &AecMemoryPtr->NvKey.TxEq;
    p_reserved_NvKey = &AecMemoryPtr->NvKey.ResKey;

    /* TX EQ use this global variable */
    gDspAlgParameter.EscoMode.Rx = mSBC;
    AEC_NR_NVKEY_Init(gDspAlgParameter.EscoMode.Rx);

    EXT_POST_EC_Init(p_ecnr_bss, p_ecnr_NvKey);

    tx_nr_ptr = stream_function_get_working_buffer(para);
    if (gDspAlgParameter.EscoMode.Rx == CVSD) {
        IGO_NR_Init(tx_nr_ptr, (uint32_t *)p_reserved_NvKey, p_txeq_NvKey, _NB);
    } else {
        IGO_NR_Init(tx_nr_ptr, (uint32_t *)p_reserved_NvKey, p_txeq_NvKey, _WB);
    }

    DSP_MW_LOG_I("[DSP][AEC/NR] post init done", 0);

    return FALSE;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_ecnr_post_process(void *para)
{
    uint8_t post_ec_gain;
    uint16_t Length;
    int16_t *InBufLeft, *OutBuf;
    void *tx_nr_ptr;

    if (AEC_NR_MemCheck() == FALSE) {
        stream_feature_reinitialize(para);
        DSP_MW_LOG_I("[DSP][AEC/NR] reinitialize the stream feature list", 0);
        return FALSE;
    }

    Length = (uint16_t)stream_function_get_output_size(para);
    if (Length == 0) {
        DSP_MW_LOG_W("[DSP][AEC/NR] bypass the Post EC / NR process", 0);
        return FALSE;
    }
    if (Length != AEC_WB_FRAME_SIZE) {
        DSP_MW_LOG_E("[DSP][AEC/NR] frame size invalid, expect: 480, actual: %d", 1, Length);
        return TRUE;
    }

    InBufLeft = (int16_t *)stream_function_get_1st_inout_buffer(para);
    OutBuf = (int16_t *)(AecMemoryPtr->AecOut);

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8 *)InBufLeft, Length, AUDIO_ENHANCEMENT_IN_L);
#endif

    if ((AecMemoryPtr->NvKey.AecNr.ENABLE == 0) || (gAecNrEn_MP == FALSE))
    {
        //DSP_MW_LOG_I("[AEC] Disable", 0);
        return FALSE;
    }

#if defined(AIR_IGO_NR_ENABLE)
#if 1
    uint32_t gpt_begin, gpt_end, gpt_during;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_begin);
    tx_nr_ptr = stream_function_get_working_buffer(para);
    IGO_NR_Prcs(InBufLeft, OutBuf, tx_nr_ptr);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_end);
    hal_gpt_get_duration_count(gpt_begin, gpt_end, &gpt_during);
    DSP_MW_LOG_W("[DSP][AEC/NR] the Post EC / NR process, time %d us", 1, gpt_during);
#else
    hal_gpt_delay_us(3000); /* Run with 416M in DSP side */
    memcpy(OutBuf, InBufLeft, Length);
#endif
#endif

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)OutBuf, Length, AUDIO_ENHANCEMENT_OUT_L);
#endif

    /* Using the end of stream buffer for extra data transmit */
    post_ec_gain = AEC_NR_Port_Get_PostEC_Gain();
    EXT_POST_EC_PRCS(p_ecnr_bss, (S16 *)OutBuf, InBufLeft, post_ec_gain, &(gDspAlgParameter.AecNr));

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)InBufLeft, Length, VOICE_TX_NR_OUT);
#endif

    return FALSE;
}

#endif
#endif

