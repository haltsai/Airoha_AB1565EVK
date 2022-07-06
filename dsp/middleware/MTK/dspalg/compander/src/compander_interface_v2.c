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

#include "dsp_feature_interface.h"
#include "dsp_audio_process.h"
#include "dsp_para_cpd.h"
#include "dsp_share_memory.h"
#include "compander_interface.h"
#include "dsp_gain_control.h"
#include "dsp_rom_table.h"
#include "dsp_audio_ctrl.h"
#include "dsp_control.h"
#include "dsp_memory.h"
#include "audio_nvdm_common.h"
#include "dsp_dump.h"
#include "source_inter.h"

//current cpd version: 20190625 for 88.2kHz

#ifdef MTK_BT_A2DP_CPD_USE_PIC
#include "cpd_portable.h"
#endif

/*
 * Buffer Control
*/
VO_CPD_INSTANCE_ptr VoRxCpdMemoryPtr = NULL;
VO_CPD_INSTANCE_ptr VoTxCpdMemoryPtr = NULL;
AU_CPD_INSTANCE_ptr Au1CpdMemoryPtr = NULL;
AU_CPD_INSTANCE_ptr Au2CpdMemoryPtr = NULL;
AU_CPD_INSTANCE_ptr Au3CpdMemoryPtr = NULL;


/**
 *
 *  Type Enum
 *
 */

typedef enum
{
    MODE_NB_TX_CPD  = 0,
    MODE_WB_TX_CPD  = 1,
    MODE_UWB_TX_CPD = 2,
    MODE_VO_TX      = MODE_UWB_TX_CPD,
    MODE_NB_RX_CPD  = 4,
    MODE_WB_RX_CPD  = 5,
    MODE_UWB_RX_CPD = 6,
    MODE_VP_CPD     = 20,
    MODE_VO_RX      = MODE_VP_CPD,
} eCPD_VO_MODE;

typedef enum
{
    MODE_AU1_CPD = 0,
    MODE_AU2_CPD = 1,
    MODE_AU3_CPD = 2,
} eCPD_AU_MODE;

typedef enum
{
    CPD_AU  = 0,
    CPD_VO  = 1,
} eCPD_AU_VO_MODE;


#define CPD_AU_FRAME_SIZE           (32)
#define CPD_AU_FRAME_SIZE_20        (20)
#define CPD_ZERO_GAIN_INDEX         (91)

#define CPD_AU_RECOVERY_GAIN        (8)     //current: peq version 20181109_v1_for_mono_mode
#define CPD3_AU_RECOVERY_GAIN       (4)    //peq3 version 20181109_v1_for_mono_mode

//#define CPD_VALID_MEMORY_CHECK_VALUE              ((U32)0x6128800)
#define CPD_VALID_MEMORY_CHECK_VALUE              ((U32)0x6A8E04DB)
#define CPD3_VALID_MEMORY_CHECK_VALUE              ((U32)0x9A9A9A9A)
#define CPD_RUNTIME_INVALID_MEMORY_CHECK_VALUE    ((U32)0x5A5A5A5A)
#define CPD_RUNTIME_LINEIN_INVALID_MEMORY_CHECK_VALUE    ((U32)0x8A8A8A8A)
#define CPD_VO_RX_VALID_MEMORY_CHECK_VALUE        ((U32)0x6A6A6A6A)
#define CPD_VO_TX_VALID_MEMORY_CHECK_VALUE        ((U32)0x7A7A7A7A)
#define CPD_AU_MEMSIZE          (sizeof(AU_CPD_INSTANCE_t))
#define CPD_VO_MEMSIZE          (sizeof(VO_CPD_INSTANCE_t))
#ifdef AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE
extern uint32_t DRC_DGAIN;
#endif

static AU_CPD_CTRL_t aud_cpd_ctrl =
{
    .enable = 0,
    .trigger_drc_change = 1,
    .force_disable = 0,
    .audio_path = 0,
    .peq2_nvkey_id = 0,
    .peq2_nvkey_id_backup = 0,
};

static AU_CPD_CTRL_t aud_cpd2_ctrl =
{
    .enable = 0,
    .trigger_drc_change = 1,
    .force_disable = 0,
    .audio_path = 0,
};

static AU_CPD_CTRL_t aud_cpd3_ctrl =
{
    .enable = 0,
    .trigger_drc_change = 1,
    .force_disable = 0,
    .audio_path = 1,
};

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
extern U8 peq_get_trigger_drc(U8 phase_id,U8 type);
#endif


/**
 *
 * Function Prototype
 *
 */
bool CPD_MemInit (void* para, eCPD_AU_VO_MODE au_vo_mode, eCPD_VO_MODE vo_mode, eCPD_AU_MODE au_mode);
void CPD_AU_MemAlloc(AU_CPD_INSTANCE_ptr *memoryptr, DSP_STREAMING_PARA_PTR stream_ptr);
void CPD_VO_MemAlloc(VO_CPD_INSTANCE_ptr *memoryptr, DSP_STREAMING_PARA_PTR stream_ptr);
bool CPD_MemCheck (eCPD_AU_VO_MODE au_vo_mode, eCPD_VO_MODE vo_mode, eCPD_AU_MODE au_mode);


bool stream_function_drc_voice_tx_wb_initialize (void *para);
bool stream_function_drc_voice_tx_nb_initialize (void *para);
bool stream_function_drc_voice_rx_wb_initialize (void *para);
bool stream_function_drc_voice_rx_nb_initialize (void *para);
bool stream_function_drc_voice_tx_process (void *para);
bool Voice_Prompt_CPD_Process (void *para);
bool stream_function_drc_voice_rx_process (void *para);
bool Voice_RX_NB_CPD_Process (void *para);
bool Voice_CPD_Process (void *para);
bool Voice_CPD_Mode_Init (void *para, eCPD_VO_MODE vo_mode);
VOID Voice_CPD_EC_Para_Init (VOID);

static const S16 DSPGAIN_Q11[124] =
{
    0x0,  //for mute
    0x2,  //-60.0dB
    0x3,  //-57.0dB
    0x4,  //-54.0dB
    0x5,  //-52.0dB
    0x7,  //-49.0dB
    0x8,  //-48.0dB
    0x9,  //-47.0dB
    0xA,  //-46.0dB
    0xC,  //-45.0dB
    0xD,  //-44.0dB
    0xE,  //-43.0dB
    0x10,  //-42.0dB
    0x12,  //-41.0dB
    0x14,  //-40.0dB
    0x17,  //-39.0dB
    0x1A,  //-38.0dB
    0x1D,  //-37.0dB
    0x1F,  //-36.5dB
    0x20,  //-36.0dB
    0x22,  //-35.5dB
    0x24,  //-35.0dB
    0x27,  //-34.5dB
    0x29,  //-34.0dB
    0x2B,  //-33.5dB
    0x2E,  //-33.0dB
    0x31,  //-32.5dB
    0x33,  //-32.0dB
    0x36,  //-31.5dB
    0x3A,  //-31.0dB
    0x3D,  //-30.5dB
    0x41,  //-30.0dB
    0x45,  //-29.5dB
    0x49,  //-29.0dB
    0x4D,  //-28.5dB
    0x52,  //-28.0dB
    0x56,  //-27.5dB
    0x5B,  //-27.0dB
    0x61,  //-26.5dB
    0x67,  //-26.0dB
    0x6D,  //-25.5dB
    0x73,  //-25.0dB
    0x7A,  //-24.5dB
    0x81,  //-24.0dB
    0x89,  //-23.5dB
    0x91,  //-23.0dB
    0x9A,  //-22.5dB
    0xA3,  //-22.0dB
    0xAC,  //-21.5dB
    0xB7,  //-21.0dB
    0xC1,  //-20.5dB
    0xCD,  //-20.0dB
    0xD9,  //-19.5dB
    0xE6,  //-19.0dB
    0xF3,  //-18.5dB
    0x102,  //-18.0dB
    0x111,  //-17.5dB
    0x121,  //-17.0dB
    0x132,  //-16.5dB
    0x145,  //-16.0dB
    0x158,  //-15.5dB
    0x16C,  //-15.0dB
    0x182,  //-14.5dB
    0x199,  //-14.0dB
    0x1B1,  //-13.5dB
    0x1CA,  //-13.0dB
    0x1E6,  //-12.5dB
    0x202,  //-12.0dB
    0x221,  //-11.5dB
    0x241,  //-11.0dB
    0x263,  //-10.5dB
    0x288,  //-10.0dB
    0x2AE,  //-9.5dB
    0x2D7,  //-9.0dB
    0x302,  //-8.5dB
    0x32F,  //-8.0dB
    0x360,  //-7.5dB
    0x393,  //-7.0dB
    0x3C9,  //-6.5dB
    0x402,  //-6.0dB
    0x43F,  //-5.5dB
    0x480,  //-5.0dB
    0x4C4,  //-4.5dB
    0x50C,  //-4.0dB
    0x559,  //-3.5dB
    0x5AA,  //-3.0dB
    0x600,  //-2.5dB
    0x65B,  //-2.0dB
    0x6BB,  //-1.5dB
    0x721,  //-1.0dB
    0x78D,  //-0.5dB
    0x800,  //0.0dB
    0x879,  //0.5dB
    0x8FA,  //1.0dB
    0x982,  //1.5dB
    0xA12,  //2.0dB
    0xAAB,  //2.5dB
    0xB4D,  //3.0dB
    0xBF8,  //3.5dB
    0xCAE,  //4.0dB
    0xD6E,  //4.5dB
    0xE3A,  //5.0dB
    0xF12,  //5.5dB
    0xFF6,  //6.0dB
    0x10E8,  //6.5dB
    0x11E9,  //7.0dB
    0x12F9,  //7.5dB
    0x1418,  //8.0dB
    0x1549,  //8.5dB
    0x168C,  //9.0dB
    0x17E2,  //9.5dB
    0x194C,  //10.0dB
    0x1ACC,  //10.5dB
    0x1C63,  //11.0dB
    0x1E11,  //11.5dB
    0x1FD9,  //12.0dB
    0x21BC,  //12.5dB
    0x23BC,  //13.0dB
    0x25DA,  //13.5dB
    0x2818,  //14.0dB
    0x2A79,  //14.5dB
    0x2CFD,  //15.0dB
    0x2FA7,  //15.5dB
    0x327A,  //16.0dB
};

static const S32 DSPGAIN_Q11_32bit[140] =
{
    0x00000000,     //for mute
    0x00020C49,     //-60.0dB
    0x0002E493,     //-57.0dB
    0x00041617,     //-54.0dB
    0x000524F3,     //-52.0dB
    0x0007443E,     //-49.0dB
    0x0008273A,     //-48.0dB
    0x000925E8,     //-47.0dB
    0x000A43AA,     //-46.0dB
    0x000B8449,     //-45.0dB
    0x000CEC08,     //-44.0dB
    0x000E7FAC,     //-43.0dB
    0x00104491,     //-42.0dB
    0x001240B8,     //-41.0dB
    0x00147AE1,     //-40.0dB
    0x0016FA9B,     //-39.0dB
    0x0019C865,     //-38.0dB
    0x001CEDC3,     //-37.0dB
    0x001EA495,     //-36.5dB
    0x00207567,     //-36.0dB
    0x002261C4,     //-35.5dB
    0x00246B4E,     //-35.0dB
    0x002693BF,     //-34.5dB
    0x0028DCEB,     //-34.0dB
    0x002B48C4,     //-33.5dB
    0x002DD958,     //-33.0dB
    0x003090D3,     //-32.5dB
    0x00337184,     //-32.0dB
    0x00367DDC,     //-31.5dB
    0x0039B871,     //-31.0dB
    0x003D2400,     //-30.5dB
    0x0040C371,     //-30.0dB
    0x004499D6,     //-29.5dB
    0x0048AA70,     //-29.0dB
    0x004CF8B4,     //-28.5dB
    0x00518847,     //-28.0dB
    0x00565D0A,     //-27.5dB
    0x005B7B15,     //-27.0dB
    0x0060E6C0,     //-26.5dB
    0x0066A4A5,     //-26.0dB
    0x006CB9A2,     //-25.5dB
    0x00732AE1,     //-25.0dB
    0x0079FDD9,     //-24.5dB
    0x00813856,     //-24.0dB
    0x0088E078,     //-23.5dB
    0x0090FCBF,     //-23.0dB
    0x0099940D,     //-22.5dB
    0x00A2ADAD,     //-22.0dB
    0x00AC5156,     //-21.5dB
    0x00B68737,     //-21.0dB
    0x00C157FA,     //-20.5dB
    0x00CCCCCC,     //-20.0dB
    0x00D8EF66,     //-19.5dB
    0x00E5CA14,     //-19.0dB
    0x00F367BE,     //-18.5dB
    0x0101D3F2,     //-18.0dB
    0x01111AED,     //-17.5dB
    0x012149A5,     //-17.0dB
    0x01326DD7,     //-16.5dB
    0x0144960C,     //-16.0dB
    0x0157D1AE,     //-15.5dB
    0x016C310E,     //-15.0dB
    0x0181C576,     //-14.5dB
    0x0198A135,     //-14.0dB
    0x01B0D7B1,     //-13.5dB
    0x01CA7D76,     //-13.0dB
    0x01E5A847,     //-12.5dB
    0x02026F30,     //-12.0dB
    0x0220EA9F,     //-11.5dB
    0x0241346F,     //-11.0dB
    0x02636807,     //-10.5dB
    0x0287A26C,     //-10.0dB
    0x02AE025C,     //-9.5dB
    0x02D6A866,     //-9.0dB
    0x0301B70A,     //-8.5dB
    0x032F52CF,     //-8.0dB
    0x035FA26A,     //-7.5dB
    0x0392CED8,     //-7.0dB
    0x03C90386,     //-6.5dB
    0x04026E73,     //-6.0dB
    0x043F4057,     //-5.5dB
    0x047FACCF,     //-5.0dB
    0x04C3EA83,     //-4.5dB
    0x050C335D,     //-4.0dB
    0x0558C4B2,     //-3.5dB
    0x05A9DF7A,     //-3.0dB
    0x05FFC889,     //-2.5dB
    0x065AC8C2,     //-2.0dB
    0x06BB2D60,     //-1.5dB
    0x0721482B,     //-1.0dB
    0x078D6FC9,     //-0.5dB
    0x08000000,     //0.0dB
    0x08795A04,     //0.5dB
    0x08F9E4CF,     //1.0dB
    0x09820D74,     //1.5dB
    0x0A12477C,     //2.0dB
    0x0AAB0D48,     //2.5dB
    0x0B4CE07B,     //3.0dB
    0x0BF84A66,     //3.5dB
    0x0CADDC7B,     //4.0dB
    0x0D6E30CD,     //4.5dB
    0x0E39EA8E,     //5.0dB
    0x0F11B69D,     //5.5dB
    0x0FF64C16,     //6.0dB
    0x10E86CF1,     //6.5dB
    0x11E8E6A0,     //7.0dB
    0x12F892C7,     //7.5dB
    0x141857E9,     //8.0dB
    0x15492A38,     //8.5dB
    0x168C0C59,     //9.0dB
    0x17E21048,     //9.5dB
    0x194C583A,     //10.0dB
    0x1ACC179A,     //10.5dB
    0x1C629405,     //11.0dB
    0x1E112669,     //11.5dB
    0x1FD93C1F,     //12.0dB
    0x21BC5829,     //12.5dB
    0x23BC1478,     //13.0dB
    0x25DA2345,     //13.5dB
    0x28185086,     //14.0dB
    0x2A78836F,     //14.5dB
    0x2CFCC016,     //15.0dB
    0x2FA72923,     //15.5dB
    0x327A01A4,     //16.0dB
    0x3577AEF5,     //16.5dB
    0x38A2BACB,     //17.0dB
    0x3BFDD55A,     //17.5dB
    0x3F8BD79D,     //18.0dB
    0x434FC5C2,     //18.5dB
    0x474CD1B7,     //19.0dB
    0x4B865DE3,     //19.5dB
    0x50000000,     //20.0dB
    0x54BD842B,     //20.5dB
    0x59C2F01D,     //21.0dB
    0x5F14868E,     //21.5dB
    0x64B6CADC,     //22.0dB
    0x6AAE84D8,     //22.5dB
    0x7100C4D7,     //23.0dB
    0x77B2E7FF,     //23.5dB
    0x7ECA9CD2,     //24.0dB
};

#define Q11_TABLE_SIZE (sizeof(DSPGAIN_Q11)/sizeof(DSPGAIN_Q11[0]))

/**
 * DSP_GC_ConvertQ11Form_32bit
 *
 * Convert table value to Q11 format
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 *
 *
 */

static S16 DSP_GC_ConvertQ11Form (S16 Value)
{
    configASSERT(Value < (S16)Q11_TABLE_SIZE);
    return DSPGAIN_Q11[Value];
}

static S32 DSP_GC_ConvertQ11Form_32bit (S16 Value)
{
    configASSERT(Value < (S16)Q11_TABLE_SIZE);
    return DSPGAIN_Q11_32bit[Value];
}

/**
 * CPD_MemInit
 *
 * This function is used to init memory space for CPD process
 *
 *
 * @para : Default parameter of callback function
 * @return : Initialize result
 */
bool CPD_MemInit (void* para, eCPD_AU_VO_MODE au_vo_mode, eCPD_VO_MODE vo_mode, eCPD_AU_MODE au_mode)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    if (!CPD_MemCheck(au_vo_mode, vo_mode, au_mode))
    {
        if(au_vo_mode == CPD_AU) {
            if (au_mode == MODE_AU1_CPD) {
                if (!(Au1CpdMemoryPtr && (Au1CpdMemoryPtr->MemoryCheck == CPD_RUNTIME_INVALID_MEMORY_CHECK_VALUE))) {
                   Au1CpdMemoryPtr = (AU_CPD_INSTANCE_ptr)DSPMEM_tmalloc(DAV_TASK_ID, CPD_AU_MEMSIZE+get_CPD_memsize(CPD_AU), stream_ptr);
                }
                Au1CpdMemoryPtr->MemoryCheck = CPD_VALID_MEMORY_CHECK_VALUE;
                return FALSE;

            } else if (au_mode == MODE_AU2_CPD) {
                if (!(Au2CpdMemoryPtr && (Au2CpdMemoryPtr->MemoryCheck == CPD_RUNTIME_INVALID_MEMORY_CHECK_VALUE))) {
                   Au2CpdMemoryPtr = (AU_CPD_INSTANCE_ptr)DSPMEM_tmalloc(DAV_TASK_ID, CPD_AU_MEMSIZE+get_CPD_memsize(CPD_AU), stream_ptr);
                }
                Au2CpdMemoryPtr->MemoryCheck = CPD_VALID_MEMORY_CHECK_VALUE;
                return FALSE;
            } else if (au_mode == MODE_AU3_CPD) {
                if (!(Au3CpdMemoryPtr && (Au3CpdMemoryPtr->MemoryCheck == CPD_RUNTIME_LINEIN_INVALID_MEMORY_CHECK_VALUE))) {
                   Au3CpdMemoryPtr = (AU_CPD_INSTANCE_ptr)DSPMEM_tmalloc(DAV_TASK_ID, CPD_AU_MEMSIZE+get_CPD_memsize(CPD_AU), stream_ptr);
                }
                Au3CpdMemoryPtr->MemoryCheck = CPD3_VALID_MEMORY_CHECK_VALUE;
                return FALSE;
            } else {
                DSP_MW_LOG_W("[CPD_MemInit] No this type of AU CPD : %d", 1, au_mode);
            }

        } else if (au_vo_mode == CPD_VO) {
            if (vo_mode <= MODE_VO_TX) {
                if (!(VoTxCpdMemoryPtr && (VoTxCpdMemoryPtr->MemoryCheck == CPD_VO_TX_VALID_MEMORY_CHECK_VALUE))) {
                   VoTxCpdMemoryPtr = (VO_CPD_INSTANCE_ptr)DSPMEM_tmalloc(DAV_TASK_ID, CPD_VO_MEMSIZE+get_CPD_memsize(CPD_VO), stream_ptr);
                }
                VoTxCpdMemoryPtr->MemoryCheck = CPD_VO_TX_VALID_MEMORY_CHECK_VALUE;
                return FALSE;

            } else if ((vo_mode > MODE_VO_TX) && (vo_mode <= MODE_VO_RX)) {
                if (!(VoRxCpdMemoryPtr && (VoRxCpdMemoryPtr->MemoryCheck == CPD_VO_RX_VALID_MEMORY_CHECK_VALUE))) {
                   VoRxCpdMemoryPtr = (VO_CPD_INSTANCE_ptr)DSPMEM_tmalloc(DAV_TASK_ID, CPD_VO_MEMSIZE+get_CPD_memsize(CPD_VO), stream_ptr);
                }
                VoRxCpdMemoryPtr->MemoryCheck = CPD_VO_RX_VALID_MEMORY_CHECK_VALUE;

                return FALSE;
            } else {
                DSP_MW_LOG_W("[CPD_MemInit] No this type of VO CPD : %d", 1, vo_mode);
            }

        } else {
            DSP_MW_LOG_W("[CPD_MemInit] No this type of CPD : %d", 1, au_vo_mode);
        }
    }

    return TRUE;
}



/**
 * CPD_MemCheck
 *
 * This function is used to check init memory space for CPD process
 *
 *
 * @para : Default parameter of callback function
 * @return : Check result
 */
bool CPD_MemCheck (eCPD_AU_VO_MODE au_vo_mode, eCPD_VO_MODE vo_mode, eCPD_AU_MODE au_mode)
{
    if(au_vo_mode == CPD_AU) {
        if(au_mode == MODE_AU1_CPD) {
            if(NULL != Au1CpdMemoryPtr) {
                if(CPD_VALID_MEMORY_CHECK_VALUE == Au1CpdMemoryPtr->MemoryCheck) {
                    return TRUE;
                }
            }
        } else if(au_mode == MODE_AU2_CPD) {
            if(NULL != Au2CpdMemoryPtr) {
                if(CPD_VALID_MEMORY_CHECK_VALUE == Au2CpdMemoryPtr->MemoryCheck) {
                    return TRUE;
                }
            }
        } else if(au_mode == MODE_AU3_CPD) {
            if(NULL != Au3CpdMemoryPtr) {
                if(CPD3_VALID_MEMORY_CHECK_VALUE == Au3CpdMemoryPtr->MemoryCheck) {
                    return TRUE;
                }
            }
        } else {
            DSP_MW_LOG_W("[CPD Mem Check] No this type of AU CPD : %d", 1, au_mode);
        }

    } else if(au_vo_mode == CPD_VO) {
        if(vo_mode <= MODE_VO_TX) {
            if(NULL != VoTxCpdMemoryPtr) {
                if(CPD_VO_TX_VALID_MEMORY_CHECK_VALUE == VoTxCpdMemoryPtr->MemoryCheck) {
                    return TRUE;
                }
            }
        } else if((vo_mode > MODE_VO_TX) && (vo_mode <= MODE_VO_RX)) {
            if(NULL != VoRxCpdMemoryPtr) {
                if(CPD_VO_RX_VALID_MEMORY_CHECK_VALUE == VoRxCpdMemoryPtr->MemoryCheck) {
                    DSP_MW_LOG_W("[CPD Mem Check] VoRxCpdMemoryPtr valid",0);
                    return TRUE;
                } else {
                    DSP_MW_LOG_W("[CPD Mem Check] VoRxCpdMemoryPtr in-valid",0);
                }
            }
        } else {
            DSP_MW_LOG_W("[CPD Mem Check] No this type of VO CPD : %d", 1, vo_mode);
        }
    } else {
        DSP_MW_LOG_W("[CPD Mem Check] No this type of CPD : %d", 1, au_vo_mode);
    }

    return FALSE;
}


void stream_function_cpd_deinitialize (void)
{
    if (Au1CpdMemoryPtr) {
        Au1CpdMemoryPtr->MemoryCheck = CPD_RUNTIME_INVALID_MEMORY_CHECK_VALUE;
    }
    if (Au2CpdMemoryPtr) {
        Au2CpdMemoryPtr->MemoryCheck = CPD_RUNTIME_INVALID_MEMORY_CHECK_VALUE;
    }
    if (Au3CpdMemoryPtr) {
        Au3CpdMemoryPtr->MemoryCheck = CPD_RUNTIME_LINEIN_INVALID_MEMORY_CHECK_VALUE;
    }
    if (VoRxCpdMemoryPtr) {
        VoRxCpdMemoryPtr->MemoryCheck = CPD_RUNTIME_INVALID_MEMORY_CHECK_VALUE;
    }
    if (VoTxCpdMemoryPtr) {
        VoTxCpdMemoryPtr->MemoryCheck = CPD_RUNTIME_INVALID_MEMORY_CHECK_VALUE;
    }
}


/**
 * stream_function_drc_voice_tx_wb_initialize
 *
 * This function is used to init memory space for voice TX CPD
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_drc_voice_tx_wb_initialize (void *para)
{
    Voice_CPD_EC_Para_Init();
    Voice_CPD_Mode_Init(para, MODE_WB_TX_CPD);

    return FALSE;
}


/**
 * stream_function_drc_voice_tx_nb_initialize
 *
 * This function is used to init memory space for voice TX CPD
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_drc_voice_tx_nb_initialize (void *para)
{
    Voice_CPD_EC_Para_Init();
    Voice_CPD_Mode_Init(para, MODE_NB_TX_CPD);

    return FALSE;
}




/**
 * stream_function_drc_voice_rx_wb_initialize
 *
 * This function is used to init memory space for voice RX CPD
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_drc_voice_rx_wb_initialize (void *para)
{
    Voice_CPD_EC_Para_Init();
    switch (gDspAlgParameter.EscoMode.Rx)
    {
        case CVSD:
            DSP_MW_LOG_D("RX NB CPD init start (cvsd)", 0);
            Voice_CPD_Mode_Init(para, MODE_NB_RX_CPD);
            break;
        case mSBC:
            DSP_MW_LOG_D("RX WB CPD init start (mSBC)", 0);
            Voice_CPD_Mode_Init(para, MODE_WB_RX_CPD);
            break;
        default:
            Voice_CPD_Mode_Init(para, MODE_WB_RX_CPD);
            break;
    }

    return FALSE;
}


/**
 * stream_function_drc_voice_rx_nb_initialize
 *
 * This function is used to init memory space for voice RX CPD
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_drc_voice_rx_nb_initialize (void *para)
{
    Voice_CPD_EC_Para_Init();
    Voice_CPD_Mode_Init(para, MODE_NB_RX_CPD);

    return FALSE;
}


/**
 * Voice_VP_CPD_Init
 *
 * This function is used to init memory space for VP CPD
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool Voice_VP_CPD_Init (void *para)
{
    Voice_CPD_Mode_Init(para, MODE_VP_CPD);

    return FALSE;
}



/**
 * stream_function_drc_voice_tx_process
 *
 * voice TX CPD main process
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_drc_voice_tx_process (void *para)
{
    S16* Buf = (S16*)stream_function_get_1st_inout_buffer(para);
    U16 FrameSize = stream_function_get_output_size(para);
    S16 Gain;
    void * p_scratch = &VoTxCpdMemoryPtr->ScratchMemory[0];

    Gain = 0x0800; /* Voice TX don't care about this parameter */

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)Buf, (U32)FrameSize, VOICE_TX_CPD_IN);
#endif

    compander_VO_proc(p_scratch,
                      Buf,
                      FrameSize/sizeof(S16),
                      Gain,
                      &gDspAlgParameter.AecNr);

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)Buf, (U32)FrameSize, VOICE_TX_CPD_OUT);
    LOG_AUDIO_DUMP((U8*)Buf, (U32)FrameSize, VOICE_TX_OUT);
#endif

    return FALSE;
}


/**
 * Voice_Prompt_CPD_Process
 *
 * voice prompt CPD wrapper function
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool Voice_Prompt_CPD_Process (void *para)
{
    return Voice_CPD_Process(para);
}


/**
 * stream_function_drc_voice_rx_process
 *
 * voice RX CPD wrapper function
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_drc_voice_rx_process (void *para)
{
    return Voice_CPD_Process(para);
}


/**
 * Voice_RX_NB_CPD_Process
 *
 * voice RX NB CPD wrapper function
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool Voice_RX_NB_CPD_Process (void *para)
{
    return Voice_CPD_Process(para);
}



/**
 * Voice_CPD_Process
 *
 * voice CPD main process
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool Voice_CPD_Process (void *para)
{
    S16* Buf = (S16*)stream_function_get_1st_inout_buffer(para);
    U16 FrameSize = stream_function_get_output_size(para);
    S16 Gain;
    void * p_scratch = &VoRxCpdMemoryPtr->ScratchMemory[0];

    Gain = DSP_GC_ConvertQ11Form(CPD_ZERO_GAIN_INDEX + VoRxCpdMemoryPtr->positive_gain);
    /*Voice_CPD_EC_Para_Init();
    Gain = 0x0800;*/

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)Buf, (U32)FrameSize, VOICE_RX_CPD_IN);
#endif

    compander_VO_proc(p_scratch,
                      Buf,
                      FrameSize/sizeof(S16),
                      Gain,
                      &gDspAlgParameter.AecNr);

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)Buf, (U32)FrameSize, VOICE_RX_CPD_OUT);
    LOG_AUDIO_DUMP((U8*)Buf, (U32)FrameSize, VOICE_RX_OUT);
#endif

    U16 i;
    for (i = 1 ; i < stream_function_get_device_channel_number(para) ; i++)
    {
        memcpy(stream_function_get_inout_buffer(para, i+1),
               stream_function_get_inout_buffer(para, 1),
               stream_function_get_output_size(para));
    }

    return FALSE;
}



/**
 * Voice_CPD_Mode_Init
 *
 * This function is used to init voice CPD with mode settings
 *
 *
 * @para : Default parameter of callback function
 *
 */
EXTERN CPD_VO_NVKEY_STATE VO_NvKey;
bool Voice_CPD_Mode_Init (void *para, eCPD_VO_MODE vo_mode)
{
    U16 KeyId;
    void * p_nvkey;
    void * p_scratch;
    if (CPD_MemInit(para, CPD_VO, vo_mode, NULL))
    {
        return FALSE;
    }
    if ((vo_mode == MODE_WB_RX_CPD) || (vo_mode == MODE_NB_RX_CPD)) {
        POSITIVE_GAIN_NVKEY_t positive_gain_nvkey;
        nvkey_read_full_key(NVKEY_DSP_PARA_POSITIVE_GAIN, &positive_gain_nvkey, sizeof(POSITIVE_GAIN_NVKEY_t));
        VoRxCpdMemoryPtr->positive_gain = (vo_mode == MODE_WB_RX_CPD) ? positive_gain_nvkey.vo_rx_wb_positive_gain : positive_gain_nvkey.vo_rx_nb_positive_gain;
        //DSP_MW_LOG_I("RX CPD positive_gain index:%d, mode:%d", 2, VoRxCpdMemoryPtr->positive_gain, vo_mode);
    }

    switch (vo_mode)
    {
        case MODE_NB_TX_CPD:
            KeyId = NVKEY_DSP_PARA_NB_TX_VO_CPD;
            p_nvkey = &VoTxCpdMemoryPtr->NvKey;
            p_scratch = &VoTxCpdMemoryPtr->ScratchMemory[0];
            break;

        case MODE_WB_TX_CPD:
            KeyId = NVKEY_DSP_PARA_WB_TX_VO_CPD;
            p_nvkey = &VoTxCpdMemoryPtr->NvKey;
            p_scratch = &VoTxCpdMemoryPtr->ScratchMemory[0];
            break;

        case MODE_NB_RX_CPD:
            KeyId = NVKEY_DSP_PARA_NB_RX_VO_CPD;
            vo_mode = MODE_WB_RX_CPD;  // use NB NVKEY but WB CPD Prc
            p_nvkey = &VoRxCpdMemoryPtr->NvKey;
            p_scratch = &VoRxCpdMemoryPtr->ScratchMemory[0];
            break;

        case MODE_WB_RX_CPD:
            KeyId = NVKEY_DSP_PARA_WB_RX_VO_CPD;
            p_nvkey = &VoRxCpdMemoryPtr->NvKey;
            p_scratch = &VoRxCpdMemoryPtr->ScratchMemory[0];
            break;

        case MODE_VP_CPD:
            KeyId = NVKEY_DSP_PARA_VP_CPD;
            p_nvkey = &VoRxCpdMemoryPtr->NvKey;
            p_scratch = &VoRxCpdMemoryPtr->ScratchMemory[0];
            break;

        default:
            KeyId = NVKEY_DSP_PARA_WB_RX_VO_CPD;
            p_nvkey = &VoRxCpdMemoryPtr->NvKey;
            p_scratch = &VoRxCpdMemoryPtr->ScratchMemory[0];
            break;
    }



    #if (CPD_TEST_DEFAULT_PARA)
    p_nvkey = &VO_NvKey;
    #else
    nvkey_read_full_key(KeyId, p_nvkey, sizeof(CPD_VO_NVKEY_STATE));
    #endif

    compander_VO_init(p_scratch, p_nvkey, vo_mode);

    return TRUE;
}


/**
 * Voice_CPD_EC_Para_Init
 *
 * This function is used to init parameters for voice CPD
 *
 *
 * @para : Default parameter of callback function
 *
 */
VOID Voice_CPD_EC_Para_Init (VOID)
{
    gDspAlgParameter.AecNr.TX_GAIN = 0x0800;
    gDspAlgParameter.AecNr.CPD_VAD = 0;
    gDspAlgParameter.AecNr.avc_vol = 0x2000;
}


/**
 * Audio_CPD_Enable
 *
 * This function is used to enable/disable audio compander.
 *
 *
 * @enable : enable or disable
 * @phase_id : for DRC or DRC2
 */
void Audio_CPD_Enable (U8 enable, U8 phase_id, U8 force_disable, U8 type, U16 peq_nvkey_id)
{
    if ((force_disable == 0) || (force_disable == 1)) {
        if (phase_id == 0) {
            if(type == 0) {
                aud_cpd_ctrl.force_disable = force_disable;
            }else {
                aud_cpd3_ctrl.force_disable = force_disable;
            }
        }
        else if (phase_id == 1) {
            aud_cpd2_ctrl.force_disable = force_disable;
            aud_cpd_ctrl.peq2_nvkey_id = peq_nvkey_id;
        }
    }
    if (phase_id == 0) {
        if(type == 0) {
            aud_cpd_ctrl.enable = enable;
        }else{
            aud_cpd3_ctrl.enable = enable;
        }
    } else if (phase_id == 1) {
        aud_cpd2_ctrl.enable = enable;
        aud_cpd_ctrl.peq2_nvkey_id = peq_nvkey_id;
    } else {
        DSP_MW_LOG_E("[Audio_CPD_Enable] Un-supported phase id: %d\n", 1, phase_id);
        return;
    }
}

static S32 Audio_CPD_Get_Fs(U8 SampleRate)
{
    //band_sw = 0(48K), 1(44.1K), 2(96K), 3(88.2K)
    S32 band_sw;
    switch(SampleRate)
    {
        case FS_RATE_48K:
            band_sw = 0;
            break;
        case FS_RATE_44_1K:
            band_sw = 1;
            break;
        case FS_RATE_96K:
            band_sw = 2;
            break;
        case FS_RATE_88_2K:
            band_sw = 3;
            break;
        default:
            band_sw = 0;
            break;
    }
    return band_sw;
}

/**
 * Audio_CPD_Algorithm_Init
 *
 * This function is used to init memory space for audio CPD
 *
 *
 * @para : Default parameter of callback function
 * @recovery_gain : shift bits to recover gain from feature ahead.
 *
 */
static bool Audio_CPD_Algorithm_Init (void *para, AU_CPD_CTRL_t *ctrl, U16 recovery_gain, AU_CPD_INSTANCE_ptr au_cpd_instance, DSP_ALG_NVKEY_e nvkey_id)
{
    void * p_scratch = &au_cpd_instance->ScratchMemory[0];
    void * p_nvkey = &au_cpd_instance->NvKey;

    U8 SampleRate = stream_function_get_samplingrate(para);

    nvkey_read_full_key(nvkey_id, p_nvkey, sizeof(CPD_AU_NVKEY_STATE));

    ctrl->sample_rate = SampleRate;
    ctrl->enabled = ctrl->enable;
    aud_cpd_ctrl.peq2_nvkey_id_backup = aud_cpd_ctrl.peq2_nvkey_id;
    
    ctrl->sample_base = CPD_AU_FRAME_SIZE;
#ifdef AIR_BT_CODEC_BLE_ENABLED
    if (((DSP_ENTRY_PARA_PTR)para)->number.field.source_type == SOURCE_TYPE_N9BLE) {
        ctrl->sample_base = CPD_AU_FRAME_SIZE_20;
    }
#endif
#ifdef MTK_CELT_DEC_ENABLE
        if (stream_function_get_decoder_type(para) == CODEC_DECODER_CELT_HD) {
            ctrl->sample_base = CPD_AU_FRAME_SIZE_20;
            DSP_MW_LOG_I("DRC init with frame size 20",0);
        }
#endif


    if (ctrl->sample_base == CPD_AU_FRAME_SIZE_20) {
        compander_AU_SetFrame120_init(p_scratch); /* set the process unit to 20 sample */
    }
    //CPD_AU_NVKEY_ptr->CPD_AU_MODE = 0x1; //dual-band
    au_cpd_instance->NvKey.band_sw = Audio_CPD_Get_Fs(SampleRate);
    au_cpd_instance->NvKey.ENABLE = (ctrl->force_disable == 1) ? 0 : 1;
    if ((au_cpd_instance->NvKey.CPD_AU_MODE & 0x1) && (au_cpd_instance->NvKey.band_sw > 1)) {
        DSP_MW_LOG_E("AU DRC doesn't support dual-band mode for 88.2/96kHz, so force to single-band mode",0);
        au_cpd_instance->NvKey.CPD_AU_MODE &= (~0x1);
    }
    compander_AU_init(p_scratch, p_nvkey, recovery_gain, au_cpd_instance->NvKey.band_sw);

    DSP_MW_LOG_I("DRC init ver: %d, fs:%dkH, enable:%d, phase_id:%d, positive_gain_index:%d, band_sw:%d", 6, SVN_version(), SampleRate, ctrl->enable, ctrl->phase_id, au_cpd_instance->positive_gain, au_cpd_instance->NvKey.band_sw);

    return FALSE;
}

/**
 * stream_function_drc_audio_initialize
 *
 * This function is used to init memory space for audio CPD with PEQ
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_drc_audio_initialize (void *para)
{
    if (CPD_MemInit(para, CPD_AU, NULL, MODE_AU1_CPD))
    {
        return TRUE;
    }

    POSITIVE_GAIN_NVKEY_t positive_gain_nvkey;
    nvkey_read_full_key(NVKEY_DSP_PARA_POSITIVE_GAIN, &positive_gain_nvkey, sizeof(POSITIVE_GAIN_NVKEY_t));
    Au1CpdMemoryPtr->positive_gain = (S32)positive_gain_nvkey.au_positive_gain;

    aud_cpd_ctrl.phase_id = 0;
    if(aud_cpd_ctrl.peq2_nvkey_id == 0){
        return Audio_CPD_Algorithm_Init(para, &aud_cpd_ctrl, CPD3_AU_RECOVERY_GAIN, Au1CpdMemoryPtr, NVKEY_DSP_PARA_A2DP_AU_CPD);
    }
    return Audio_CPD_Algorithm_Init(para, &aud_cpd_ctrl, CPD_AU_RECOVERY_GAIN, Au1CpdMemoryPtr, NVKEY_DSP_PARA_A2DP_AU_CPD);
}

bool stream_function_drc_audio2_initialize (void *para)
{
    if (CPD_MemInit(para, CPD_AU, NULL, MODE_AU2_CPD))
    {
        return TRUE;
    }

    POSITIVE_GAIN_NVKEY_t positive_gain_nvkey;
    nvkey_read_full_key(NVKEY_DSP_PARA_POSITIVE_GAIN, &positive_gain_nvkey, sizeof(POSITIVE_GAIN_NVKEY_t));
    Au2CpdMemoryPtr->positive_gain = (S32)positive_gain_nvkey.au_positive_gain;

    aud_cpd2_ctrl.phase_id = 1;
    return Audio_CPD_Algorithm_Init(para, &aud_cpd2_ctrl, CPD_AU_RECOVERY_GAIN, Au2CpdMemoryPtr, NVKEY_DSP_PARA_A2DP_AU_CPD);
}

bool stream_function_drc_audio3_initialize (void *para)
{
    if (CPD_MemInit(para, CPD_AU, NULL, MODE_AU3_CPD))
    {
        return TRUE;
    }

    POSITIVE_GAIN_NVKEY_t positive_gain_nvkey;
    nvkey_read_full_key(NVKEY_DSP_PARA_POSITIVE_GAIN, &positive_gain_nvkey, sizeof(POSITIVE_GAIN_NVKEY_t));
    Au3CpdMemoryPtr->positive_gain = (S32)positive_gain_nvkey.au_positive_gain;

    aud_cpd3_ctrl.phase_id = 0;
    return Audio_CPD_Algorithm_Init(para, &aud_cpd3_ctrl, CPD3_AU_RECOVERY_GAIN, Au3CpdMemoryPtr, NVKEY_DSP_PARA_LINE_AU_CPD);
}

/**
 * Audio_PureCPD_Init
 *
 * This function is used to init memory space for pure audio CPD without PEQ
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool Audio_PureCPD_Init (void *para)
{
    return Audio_CPD_Algorithm_Init(para, &aud_cpd_ctrl, 0, Au1CpdMemoryPtr, NVKEY_DSP_PARA_A2DP_AU_CPD);
}

void CPD_Reinit(void * p_scratch, void * p_nvkey,S32 band_sw,U8 audio_path)
{
    if(audio_path){
        compander_AU_init(p_scratch, p_nvkey, CPD3_AU_RECOVERY_GAIN, band_sw);
    }
    else{
        if(aud_cpd_ctrl.peq2_nvkey_id == 0){
            compander_AU_init(p_scratch, p_nvkey, CPD3_AU_RECOVERY_GAIN, band_sw);
        }
        else{
            compander_AU_init(p_scratch, p_nvkey, CPD_AU_RECOVERY_GAIN, band_sw);
        }
    }
    DSP_MW_LOG_I("DRC re-init",0);
}
static bool drc_audio_process (void *para, AU_CPD_CTRL_t *ctrl, AU_CPD_INSTANCE_ptr CPD_AU_ptr)
{
    S32* BufL = (S32*)stream_function_get_1st_inout_buffer(para);
    S32* BufR = (S32*)stream_function_get_2nd_inout_buffer(para);
    U16 FrameSize = stream_function_get_output_size(para);
    S16 dev_channels = (S16)stream_function_get_device_channel_number(para);
    U8 SampleRate = stream_function_get_samplingrate(para);
    U16 CpdSampleSize;
    #ifndef MTK_BT_A2DP_CPD_USE_PIC
    S16 Gain;//DSPGAIN_Q11: (0x800=0dB)
    #else
    S32 Gain;//DSPGAIN_Q11: (0x08000000=0dB)
    #endif 
    S16 channel_mode = 2; // 2:stereo 1:mono
    S32 j;
    void * p_scratch = &CPD_AU_ptr->ScratchMemory[0];
    void * p_nvkey = &CPD_AU_ptr->NvKey;

    #ifndef MTK_BT_A2DP_CPD_USE_PIC
    Gain = DSP_GC_ConvertQ11Form((S16)(CPD_ZERO_GAIN_INDEX + CPD_AU_ptr->positive_gain));
    #else
    Gain = DSP_GC_ConvertQ11Form_32bit((S16)(CPD_ZERO_GAIN_INDEX + CPD_AU_ptr->positive_gain));
    #endif 

    #ifdef AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE
    Gain = DRC_DGAIN;
    #endif

    if(ctrl->sample_rate != SampleRate)
    {
        ctrl->sample_rate = SampleRate;
        CPD_AU_ptr->NvKey.band_sw = Audio_CPD_Get_Fs(SampleRate);
        if ((CPD_AU_ptr->NvKey.CPD_AU_MODE & 0x1) && (CPD_AU_ptr->NvKey.band_sw > 1)) {
            DSP_MW_LOG_E("AU DRC doesn't support dual-band mode for 88.2/96kHz, so force to single-band mode",0);
            CPD_AU_ptr->NvKey.CPD_AU_MODE &= (~0x1);
        }
        CPD_Reinit(p_scratch, p_nvkey, CPD_AU_ptr->NvKey.band_sw, ctrl->audio_path);
        DSP_MW_LOG_I("DRC re-init to %dkH",1,SampleRate);
        }

    if((aud_cpd_ctrl.peq2_nvkey_id != aud_cpd_ctrl.peq2_nvkey_id_backup)&&(aud_cpd2_ctrl.enabled)&&(aud_cpd_ctrl.peq2_nvkey_id != NVKEY_DSP_PARA_PEQ)){
        CPD_Reinit(p_scratch, p_nvkey, CPD_AU_ptr->NvKey.band_sw, ctrl->audio_path);
        aud_cpd_ctrl.peq2_nvkey_id_backup = aud_cpd_ctrl.peq2_nvkey_id;
    }

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    ctrl->trigger_drc_change = peq_get_trigger_drc(ctrl->phase_id, ctrl->audio_path);
#endif
    if ((ctrl->enabled != ctrl->enable) && (ctrl->trigger_drc_change == 1)) {
        DSP_MW_LOG_I("DRC enable: %d -> %d, NvKey.ENABLE: %d\n", 3, ctrl->enabled, ctrl->enable, CPD_AU_ptr->NvKey.ENABLE);
        ctrl->enabled = ctrl->enable;
    }
    if (CPD_AU_ptr->NvKey.ENABLE != ((ctrl->force_disable == 1) ? 0 : 1)) { //force disable DRC for low power estimation
        DSP_MW_LOG_I("DRC force disable: %d, phase_id:%d\n", 2, ctrl->force_disable,ctrl->phase_id);
        CPD_AU_ptr->NvKey.ENABLE = (ctrl->force_disable == 1) ? 0 : 1;
    }

    if (CPD_AU_ptr->NvKey.ENABLE == 0)
    {
        return FALSE;
    }
    else if ((FrameSize == 0) || (FrameSize % ctrl->sample_base)) {
        return FALSE;
    }

    if(dev_channels == 1)
    {
        BufR = NULL;
    }

    if (stream_function_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        CpdSampleSize = FrameSize >> 2;
    }
    else
    {
        CpdSampleSize = FrameSize >> 1;
        dsp_converter_16bit_to_32bit(BufL, (S16*)BufL, CpdSampleSize);
        dsp_converter_16bit_to_32bit(BufR, (S16*)BufR, CpdSampleSize);
        stream_function_modify_output_size(para, CpdSampleSize << 2);
        stream_function_modify_output_resolution(para, RESOLUTION_32BIT);
    }

    if (ctrl->enabled == 0) {
        for (j = CpdSampleSize - 1; j >= 0; j--) {
            if(aud_cpd_ctrl.peq2_nvkey_id == 0){
                BufL[j] >>= CPD3_AU_RECOVERY_GAIN;
            }
            else{
                BufL[j] >>= CPD_AU_RECOVERY_GAIN;
            }
        }
        if (BufR) {
            for (j = CpdSampleSize - 1; j >= 0; j--) {
                if(aud_cpd_ctrl.peq2_nvkey_id == 0){
                    BufR[j] >>= CPD3_AU_RECOVERY_GAIN;
                }
                else{
                    BufR[j] >>= CPD_AU_RECOVERY_GAIN;
                }
            }
        }
    }

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    if (ctrl->phase_id == 0) {
        LOG_AUDIO_DUMP((U8*)BufL, (U32)(CpdSampleSize << 2), AUDIO_CPD_IN_L);
        if (BufR) {
            LOG_AUDIO_DUMP((U8*)BufR, (U32)(CpdSampleSize << 2), AUDIO_CPD_IN_R);
        }
    }
#endif

    channel_mode = (BufR == NULL) ? 1 : 2;
    compander_AU_proc(CPD_AU_ptr->ScratchMemory,
                      BufL,
                      BufR,
                      CpdSampleSize,
                      Gain,
                      channel_mode);

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    if (ctrl->phase_id == 0) {
        LOG_AUDIO_DUMP((U8*)BufL, (U32)(CpdSampleSize << 2), AUDIO_CPD_OUT_L);
        if (BufR) {
            LOG_AUDIO_DUMP((U8*)BufR, (U32)(CpdSampleSize << 2), AUDIO_CPD_OUT_R);
        }
    }
#endif

    return FALSE;
}

/**
 * stream_function_drc_audio_process
 *
 * audio CPD main process
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_drc_audio_process (void *para)
{
    return drc_audio_process(para, &aud_cpd_ctrl, Au1CpdMemoryPtr);
}

bool stream_function_drc_audio2_process (void *para)
{
    return drc_audio_process(para, &aud_cpd2_ctrl, Au2CpdMemoryPtr);
}

bool stream_function_drc_audio3_process (void *para)
{
    return drc_audio_process(para, &aud_cpd3_ctrl, Au3CpdMemoryPtr);
}

#ifdef PRELOADER_ENABLE
bool Voice_CPD_Open  (void *para)
{
    DSP_MW_LOG_I("[PIC] Voice Compander Open", 0);
    UNUSED(para);
    return TRUE;
}

bool Voice_CPD_Close (void *para)
{
    DSP_MW_LOG_I("[PIC] Voice Compander Close", 0);
    UNUSED(para);
    return TRUE;
}

bool Audio_CPD_Open  (void *para)
{
    DSP_MW_LOG_I("[PIC] Audio Compander Open", 0);
    UNUSED(para);
    return TRUE;
}

bool Audio_CPD_Close (void *para)
{
    DSP_MW_LOG_I("[PIC] Audio Compander Close", 0);
    UNUSED(para);
    return TRUE;
}
#endif

