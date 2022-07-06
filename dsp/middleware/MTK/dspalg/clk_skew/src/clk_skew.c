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
#include "dsp_share_memory.h"
#include "compander_interface.h"
#include "bt_interface.h"
//#include "dsp_rom_table.h"
#include "dsp_audio_ctrl.h"
#include "dsp_control.h"
#include "dsp_memory.h"
#include "clk_skew.h"
#include "hal_audio_afe_control.h"
#include "hal_audio_afe_define.h"
#include "audio_afe_common.h"
#include "long_term_clk_skew.h"
#ifdef MTK_BT_CLK_SKEW_USE_PIC
#include "clk_skew_portable.h"
#endif
#include "dsp_dump.h"
#include "clk_skew_protect.h"
#ifdef MTK_BT_HFP_FORWARDER_ENABLE
#include "bt_types.h"
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "source_inter.h"
#endif

#ifdef MTK_CELT_DEC_ENABLE
#include "celt_dec_interface.h"
#endif

#include "ext_clk_skew.h"

#ifdef MTK_BT_HFP_SPE_ALG_V2
#include "bt_interface.h"
#endif

DSP_CLOCK_SKEW_CTRL_t ClkSkewULCtrl;
DSP_CLOCK_SKEW_CTRL_t ClkSkewDLCtrl;
DSP_CLOCK_SKEW_RCDC_CTRL_t ClkSkewRCDCCtrl;
DSP_CLOCK_SKEW_PCDC_CTRL_t ClkSkewPCDCCtrl;
unsigned int ClkSkewLagsNo;
unsigned int ClkSkewLeadsNo;
int pcdc_asi_threshold;
RCDC_CLK_OFFSET_INFO_t * rcdc_clk_offset_info_ptr;
RCDC_BT_CLK_INFO_t * rcdc_clk_info_ptr;
bool fgULRCDCPolling = false;
bool fgUL1stRCDC;
#ifdef ENABLE_HWSRC_CLKSKEW
bool ClkSkewMode_g;
bool ClkSkewMode_isModify_g = false;
DSP_CLOCK_SKEW_RCDC_CTRL_t ClkSkewULRCDCCtrl;
S16 rcdc_hfp_dl_clk_idx;
#endif

/**
 *
 * Function Prototype
 *
 */
#ifdef ENABLE_HWSRC_CLKSKEW
void ClkSkewMode_Selection(hal_ccni_message_t msg, hal_ccni_message_t *ack);
#endif
CLK_SKEW_FS_t clk_skew_fs_converter(stream_samplerate_t fs_in);
#ifdef MTK_BT_HFP_SPE_ALG_V2
VOID Clock_Skew_Offset_Update(BT_CLOCK_OFFSET_SCENARIO type);
#else
VOID Clock_Skew_Offset_Update(void);
#endif
#ifdef ENABLE_HWSRC_CLKSKEW
VOID Clock_Skew_UL_Offset_Update(BT_CLOCK_OFFSET_SCENARIO type);
#endif
S16 clk_skew_check_status (DSP_CLOCK_SKEW_CTRL_t* pClkSkewCtrl, CLK_SKEW_DIRECTION_TYPE_t type);
VOID clk_skew_finish_compensation (DSP_CLOCK_SKEW_CTRL_t* pClkSkewCtrl);
bool stream_function_clock_skew_uplink_initialize (void *para);
bool stream_function_clock_skew_downlink_initialize (void *para);
bool stream_function_clock_skew_uplink_process (void *para);
bool stream_function_clock_skew_downlink_process (void *para);
bool stream_function_clock_skew_hfp_downlink_process (void *para);
bool Clock_Skew_DL_Main_Process (void *para, CLK_SKEW_DL_TYPE_t type);
bool HWSRC_Clock_Skew_Main_Process (void *para, CLK_SKEW_DL_TYPE_t type);
void clk_skew_get_ul_accumulated_bytes(void);
S16 clk_skew_get_ul_comp_samples (VOID);


#ifdef ENABLE_HWSRC_CLKSKEW
void ClkSkewMode_Selection(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    ClkSkewMode_isModify_g = TRUE;

    switch(msg.ccni_message[1])
    {
        case CLK_SKEW_V1:
          DSP_MW_LOG_I("[DSP]clkskew v1", 0);
          ClkSkewMode_g = CLK_SKEW_V1;
          break;
        case CLK_SKEW_V2:
          DSP_MW_LOG_I("[DSP]clkskew v2", 0);
          ClkSkewMode_g = CLK_SKEW_V2;
          break;
    }
}
#endif

ATTR_TEXT_IN_IRAM_LEVEL_2 CLK_SKEW_FS_t clk_skew_fs_converter(stream_samplerate_t fs_in)
{
    CLK_SKEW_FS_t fs_out;

    switch (fs_in)
    {
        case FS_RATE_8K:
            fs_out = CLK_SKEW_FS_8K;
            break;
        case FS_RATE_16K:
            fs_out = CLK_SKEW_FS_16K;
            break;
        case FS_RATE_24K:
            fs_out = CLK_SKEW_FS_24K;
            break;
        case FS_RATE_32K:
            fs_out = CLK_SKEW_FS_32K;
            break;
        case FS_RATE_44_1K:
            fs_out = CLK_SKEW_FS_44_1K;
            break;
        case FS_RATE_48K:
            fs_out = CLK_SKEW_FS_48K;
            break;
        case FS_RATE_88_2K:
            fs_out = CLK_SKEW_FS_88_2K;
            break;
        case FS_RATE_96K:
            fs_out = CLK_SKEW_FS_96K;
            break;
        case FS_RATE_192K:
            fs_out = CLK_SKEW_FS_192K;
            break;
        default:
            fs_out = 0;
            break;
    }
    return fs_out;
}

#ifdef MTK_BT_HFP_SPE_ALG_V2
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID Clock_Skew_Offset_Update(BT_CLOCK_OFFSET_SCENARIO type)
#else
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID Clock_Skew_Offset_Update(void)
#endif
{
    uint32_t mask;
    #ifdef MTK_BT_HFP_SPE_ALG_V2
    uint32_t bt_clk_offset;
    uint16_t intra_clk_offset;
    uint32_t native_clk;
    uint32_t audio_clk;
    #endif

    hal_nvic_save_and_set_interrupt_mask(&mask);
    #ifdef MTK_BT_HFP_SPE_ALG_V2
    MCE_Get_BtClkOffset((BTCLK *)&bt_clk_offset,(BTPHASE *)&intra_clk_offset, type);
    MCE_Get_NativeClk_from_Controller(&native_clk, &audio_clk);
    #endif

    if(ClkSkewRCDCCtrl.first_offset_flag == TRUE)
    {
       ClkSkewRCDCCtrl.first_offset_flag = FALSE;
       ClkSkewRCDCCtrl.prephase = -(AFE_GET_REG(AFE_AUDIO_BT_SYNC_MON1)&0xFFFF); // initial offset
       #ifdef MTK_BT_HFP_SPE_ALG_V2
       ClkSkewRCDCCtrl.bt_clk_offset_prev = bt_clk_offset;
       ClkSkewRCDCCtrl.bt_intra_slot_offset_prev = intra_clk_offset;
       if(native_clk != BTCLK_INVALID_CLK){
       ClkSkewRCDCCtrl.n9_clk_prev = native_clk;
       ClkSkewRCDCCtrl.aud_clk_prev = audio_clk;
       }/*else{
            DSP_MW_LOG_I("[RCDC]Get INVALID CLK",0);
       }*/
       
       #ifdef ENABLE_HWSRC_CLKSKEW
       if(ClkSkewMode_g == CLK_SKEW_V2){
           ClkSkewULRCDCCtrl.prephase = ClkSkewRCDCCtrl.prephase;
       }
       #endif
       #else
       ClkSkewRCDCCtrl.bt_clk_offset_prev = rcdc_clk_offset_info_ptr->bt_clk_offset;
       ClkSkewRCDCCtrl.bt_intra_slot_offset_prev = rcdc_clk_offset_info_ptr->bt_intra_slot_offset;
       ClkSkewRCDCCtrl.n9_clk_prev = rcdc_clk_info_ptr->native_clk;
       ClkSkewRCDCCtrl.aud_clk_prev = rcdc_clk_info_ptr->audio_clk;
       #endif
    } else {
       #ifdef MTK_BT_HFP_SPE_ALG_V2
       ClkSkewRCDCCtrl.bt_clk_offset_next = bt_clk_offset;
       ClkSkewRCDCCtrl.bt_intra_slot_offset_next = intra_clk_offset;
       if(native_clk != BTCLK_INVALID_CLK){
       ClkSkewRCDCCtrl.n9_clk_next = native_clk;
       ClkSkewRCDCCtrl.aud_clk_next = audio_clk;
       }/*else{
            DSP_MW_LOG_I("[RCDC]Get INVALID CLK",0);
       }*/
       #else
       ClkSkewRCDCCtrl.bt_clk_offset_next = rcdc_clk_offset_info_ptr->bt_clk_offset;
       ClkSkewRCDCCtrl.bt_intra_slot_offset_next = rcdc_clk_offset_info_ptr->bt_intra_slot_offset;
       ClkSkewRCDCCtrl.n9_clk_next = rcdc_clk_info_ptr->native_clk;
       ClkSkewRCDCCtrl.aud_clk_next = rcdc_clk_info_ptr->audio_clk;
       #endif
   }
   hal_nvic_restore_interrupt_mask(mask);
}

#ifdef ENABLE_HWSRC_CLKSKEW
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID Clock_Skew_UL_Offset_Update(BT_CLOCK_OFFSET_SCENARIO type)
{
    uint32_t mask;
    uint32_t bt_clk_offset;
    uint16_t intra_clk_offset;
    uint32_t native_clk;
    uint32_t audio_clk;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    MCE_Get_BtClkOffset((BTCLK *)&bt_clk_offset,(BTPHASE *)&intra_clk_offset,type);
    MCE_Get_NativeClk_from_Controller(&native_clk, &audio_clk);

    if(ClkSkewULRCDCCtrl.first_offset_flag == TRUE)
    {
       ClkSkewULRCDCCtrl.first_offset_flag = FALSE;
       ClkSkewULRCDCCtrl.bt_clk_offset_prev = bt_clk_offset;
       ClkSkewULRCDCCtrl.bt_intra_slot_offset_prev = intra_clk_offset;
       if(native_clk != BTCLK_INVALID_CLK){
       ClkSkewULRCDCCtrl.n9_clk_prev = native_clk;
       ClkSkewULRCDCCtrl.aud_clk_prev = audio_clk;
       }
    } else {
       ClkSkewULRCDCCtrl.bt_clk_offset_next = bt_clk_offset;
       ClkSkewULRCDCCtrl.bt_intra_slot_offset_next = intra_clk_offset;
       if(native_clk != BTCLK_INVALID_CLK){
       ClkSkewULRCDCCtrl.n9_clk_next = native_clk;
       ClkSkewULRCDCCtrl.aud_clk_next = audio_clk;
   }
   }
   hal_nvic_restore_interrupt_mask(mask);
   //DSP_MW_LOG_I("[AFE_UPDATE] bt_clk_offset:%d, intra_clk_offset:%d", 2, bt_clk_offset, intra_clk_offset);
   //DSP_MW_LOG_I("[AFE_UPDATE] native_clk= %d, audio_clk:%d", 2, native_clk, audio_clk);
}
#endif


#ifdef MTK_BT_CODEC_BLE_ENABLED
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID Clock_Skew_Offset_Update_BLE(SOURCE source,BT_CLOCK_OFFSET_SCENARIO type)
{
    uint32_t mask;
    #ifdef MTK_BT_HFP_SPE_ALG_V2
    uint32_t bt_clk_offset;
    uint16_t intra_clk_offset;
    uint32_t native_clk;
    uint32_t audio_clk;
    #endif

    hal_nvic_save_and_set_interrupt_mask(&mask);
    #ifdef MTK_BT_HFP_SPE_ALG_V2
    MCE_Get_BtClkOffset((BTCLK *)&bt_clk_offset,(BTPHASE *)&intra_clk_offset,type);
    MCE_Get_NativeClk_from_Controller(&native_clk, &audio_clk);
    #endif

    if(ClkSkewRCDCCtrl.first_offset_flag == TRUE && (source->streamBuffer.ShareBufferInfo.anchor_clk != 0))
    {
       ClkSkewRCDCCtrl.first_offset_flag = FALSE;
       ClkSkewRCDCCtrl.prephase = -(AFE_GET_REG(AFE_AUDIO_BT_SYNC_MON1)&0xFFFF); // initial offset
       #ifdef MTK_BT_HFP_SPE_ALG_V2
       ClkSkewRCDCCtrl.bt_clk_offset_prev = bt_clk_offset;
       ClkSkewRCDCCtrl.bt_intra_slot_offset_prev = intra_clk_offset;
       ClkSkewRCDCCtrl.n9_clk_prev = native_clk;
       ClkSkewRCDCCtrl.aud_clk_prev = audio_clk;
       #else
       ClkSkewRCDCCtrl.bt_clk_offset_prev = source->streamBuffer.ShareBufferInfo.anchor_clk;
       ClkSkewRCDCCtrl.bt_intra_slot_offset_prev = (U16)source->streamBuffer.ShareBufferInfo.asi_base;
       ClkSkewRCDCCtrl.n9_clk_prev = (U32)source->streamBuffer.ShareBufferInfo.drift_comp_val;
       ClkSkewRCDCCtrl.aud_clk_prev = source->streamBuffer.ShareBufferInfo.asi_current;
       #endif
       //printf("prev: nclk_offset:%d, intra_offset:%d, nclk:%d, aclk:%d", source->streamBuffer.ShareBufferInfo.anchor_clk, source->streamBuffer.ShareBufferInfo.asi_base, source->streamBuffer.ShareBufferInfo.drift_comp_val, source->streamBuffer.ShareBufferInfo.asi_current);
    } else {
       #ifdef MTK_BT_HFP_SPE_ALG_V2
       ClkSkewRCDCCtrl.bt_clk_offset_next = bt_clk_offset;
       ClkSkewRCDCCtrl.bt_intra_slot_offset_next = intra_clk_offset;
       ClkSkewRCDCCtrl.n9_clk_next = native_clk;
       ClkSkewRCDCCtrl.aud_clk_next = audio_clk;
       #else
       ClkSkewRCDCCtrl.bt_clk_offset_next = source->streamBuffer.ShareBufferInfo.anchor_clk;
       ClkSkewRCDCCtrl.bt_intra_slot_offset_next = (U16)source->streamBuffer.ShareBufferInfo.asi_base;
       ClkSkewRCDCCtrl.n9_clk_next = (U32)source->streamBuffer.ShareBufferInfo.drift_comp_val;
       ClkSkewRCDCCtrl.aud_clk_next = source->streamBuffer.ShareBufferInfo.asi_current;
       #endif
       //printf("next: nclk_offset:%d, intra_offset:%d, nclk:%d, aclk:%d", source->streamBuffer.ShareBufferInfo.anchor_clk, source->streamBuffer.ShareBufferInfo.asi_base, source->streamBuffer.ShareBufferInfo.drift_comp_val, source->streamBuffer.ShareBufferInfo.asi_current);
   }
   hal_nvic_restore_interrupt_mask(mask);
}
#endif


ATTR_TEXT_IN_IRAM_LEVEL_2 S16 clk_skew_check_status (DSP_CLOCK_SKEW_CTRL_t* pClkSkewCtrl, CLK_SKEW_DIRECTION_TYPE_t type)
{
    uint32_t mask;
    S16 i2AfeModifySamples = 0;

    if (!pClkSkewCtrl->Initialized) {
        return 0;
    }

    if ((pClkSkewCtrl->IntrDownCnt > 0) && (pClkSkewCtrl->CompensatedPolarity != 0)) {
        /* Audio Compensation Already Exists */
        pClkSkewCtrl->IntrDownCnt--;

        if((pClkSkewCtrl->IntrDownCnt == 1) && (type==CLK_SKEW_UL) && (pClkSkewCtrl->InterruptHandleCnt == 3) && (pClkSkewCtrl->CompensatedPolarity < 0)) {
            fgULRCDCPolling = TRUE;
        }

        if (pClkSkewCtrl->IntrDownCnt == 0) {
            if (pClkSkewCtrl->CompensatedPolarity > 0) {
                pClkSkewCtrl->CompensatedSamples = 1;
            } else {
                pClkSkewCtrl->CompensatedSamples = -1;
            }

            pClkSkewCtrl->CompensatedPolarity = 0;
        }
    } else {

        hal_nvic_save_and_set_interrupt_mask(&mask); // Protect pClkSkewCtrl->AccumulatedSamples

        /* Audio Compensation Does Not Exist */
        if ((pClkSkewCtrl->AccumulatedSamples > 0) && (pClkSkewCtrl->CompensatedSamples == 0)) {
            /* Audio Sample Leads */
            pClkSkewCtrl->AccumulatedSamples--;
            pClkSkewCtrl->IntrDownCnt = 2;

            if(type == CLK_SKEW_DL) {
                pClkSkewCtrl->CompensatedPolarity = 1;
                i2AfeModifySamples = 1;
                DSP_MW_LOG_I("[ClkSkew] DL acu +", 0);
            } else if(type == CLK_SKEW_UL) {
                pClkSkewCtrl->CompensatedPolarity = -1;
                DSP_MW_LOG_I("[ClkSkew] UL acu - :%d", 1, pClkSkewCtrl->CompensatedPolarity);

                if(pClkSkewCtrl->UL_Prev_Polarity == 0) {
                    pClkSkewCtrl->UL_Prev_Polarity = pClkSkewCtrl->CompensatedPolarity;
                    DSP_MW_LOG_I("[ClkSkew] UL first set polarity: %d", 1, pClkSkewCtrl->UL_Prev_Polarity);
                }else if(pClkSkewCtrl->UL_Prev_Polarity == 1) {
                    DSP_MW_LOG_I("[ClkSkew] UL polarity is diffrent, prev:%d, now:%d", 2, pClkSkewCtrl->UL_Prev_Polarity, pClkSkewCtrl->CompensatedPolarity);
                    pClkSkewCtrl->UL_Pol_Change_Samples = 1;
                    return 0;
                }

                if (pClkSkewCtrl->InterruptHandleCnt == 0) {
                    if(pClkSkewCtrl->UL_Pol_Change_Samples == -1) {
                        DSP_MW_LOG_I("[ClkSkew] UL_Pol_Change_Samples != 0:%d", 1, pClkSkewCtrl->UL_Pol_Change_Samples);
                        pClkSkewCtrl->UL_Pol_Change_Samples = 0;
                        return 0;
                    } else {
                        pClkSkewCtrl->InterruptHandleCnt = 3;
                        i2AfeModifySamples = 4;
                    }
                } else {
                    pClkSkewCtrl->InterruptHandleCnt--;
                    i2AfeModifySamples = 0;
                }
            }

            hal_nvic_restore_interrupt_mask(mask);
            return i2AfeModifySamples; // Report to inform afe consume 1 more sample for next next ISR

        } else if ((pClkSkewCtrl->AccumulatedSamples < 0) && (pClkSkewCtrl->CompensatedSamples == 0)) {
            /* Audio Sample Lags */
            pClkSkewCtrl->AccumulatedSamples++;
            pClkSkewCtrl->IntrDownCnt = 2;

            if(type == CLK_SKEW_DL) {
                pClkSkewCtrl->CompensatedPolarity = -1;
                i2AfeModifySamples = -1;
                DSP_MW_LOG_I("[ClkSkew] DL acu -", 0);
            }
            else if(type == CLK_SKEW_UL) {
                pClkSkewCtrl->CompensatedPolarity = 1;

                if(pClkSkewCtrl->UL_Prev_Polarity == 0) {
                    pClkSkewCtrl->UL_Prev_Polarity = pClkSkewCtrl->CompensatedPolarity;
                    DSP_MW_LOG_I("[ClkSkew] UL first set polarity: %d", 1, pClkSkewCtrl->UL_Prev_Polarity);
                }else if(-((S32)(pClkSkewCtrl->UL_Prev_Polarity)) == 1) {
                    DSP_MW_LOG_I("[ClkSkew] UL polarity is diffrent, prev:%d, now:%d", 2, pClkSkewCtrl->UL_Prev_Polarity, pClkSkewCtrl->CompensatedPolarity);
                    pClkSkewCtrl->UL_Pol_Change_Samples = -1;
                    i2AfeModifySamples = 0;
                    return i2AfeModifySamples;
                }

                if (pClkSkewCtrl->InterruptHandleCnt == 0) {
                    if(pClkSkewCtrl->UL_Pol_Change_Samples == 1) {
                        DSP_MW_LOG_I("[ClkSkew] UL_Pol_Change_Samples != 0:%d", 1, pClkSkewCtrl->UL_Pol_Change_Samples);
                        pClkSkewCtrl->UL_Pol_Change_Samples = 0;
                        return 0;
                    } else {
                        pClkSkewCtrl->InterruptHandleCnt = 3;
                        i2AfeModifySamples = 0;
                    }
                } else {
                    pClkSkewCtrl->InterruptHandleCnt--;
                    i2AfeModifySamples = 0;

                    if (pClkSkewCtrl->InterruptHandleCnt == 0) {
                        i2AfeModifySamples = -4;
                    }
                }
            }

            hal_nvic_restore_interrupt_mask(mask);
            return i2AfeModifySamples; // Report to inform afe fetch 1 less sample for next next ISR

        } else {
            /* Do nothing */
        }

        hal_nvic_restore_interrupt_mask(mask);
    }

    return 0;
}

VOID clk_skew_finish_compensation (DSP_CLOCK_SKEW_CTRL_t* pClkSkewCtrl)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    #if 0
    if (pClkSkewCtrl->CompensatedSamples > 0) {
        pClkSkewCtrl->AccumulatedSamples--;
    } else if (pClkSkewCtrl->CompensatedSamples < 0) {
        pClkSkewCtrl->AccumulatedSamples++;
    }
    #endif

    pClkSkewCtrl->CompensatedSamples = 0;
    hal_nvic_restore_interrupt_mask(mask);
}

VOID clk_skew_inform_ul_leads_samples (U32 samples)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    ClkSkewULCtrl.AccumulatedSamples += samples;
    hal_nvic_restore_interrupt_mask(mask);
}

VOID clk_skew_inform_ul_lags_samples (U32 samples)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    ClkSkewULCtrl.AccumulatedSamples -= samples;
    hal_nvic_restore_interrupt_mask(mask);
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void clk_skew_get_ul_accumulated_bytes(void)
{
    if(fgUL1stRCDC == TRUE){
        if(ClkSkewDLCtrl.AccumulatedSamples != 0) {
            fgUL1stRCDC = FALSE;
        }
    }
    else {
        ClkSkewULCtrl.AccumulatedSamples = ClkSkewDLCtrl.AccumulatedSamples;
    }
}

#ifdef ENABLE_HWSRC_CLKSKEW
ATTR_TEXT_IN_IRAM_LEVEL_2 void clk_skew_get_dl_accumulated_bytes_from_ul(U32 fs_in, U32 fs_out)
{
    UNUSED(fs_in);
    UNUSED(fs_out);
    if(ClkSkewULCtrl.AccumulatedSamples != 0) {
        ClkSkewDLCtrl.AccumulatedSamples = ClkSkewULCtrl.AccumulatedSamples;
    }
}
#endif

ATTR_TEXT_IN_IRAM_LEVEL_2 S16 clk_skew_get_ul_comp_bytes (VOID)
{
    return ClkSkewULCtrl.CompensatedSamples*ClkSkewULCtrl.BytesPerSample;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 S16 clk_skew_get_ul_comp_samples (VOID)
{
    return ClkSkewULCtrl.CompensatedSamples;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 S16 clk_skew_check_ul_status (VOID)
{
    return clk_skew_check_status(&ClkSkewULCtrl, CLK_SKEW_UL);
}

VOID clk_skew_finish_ul_compensation (VOID)
{
    clk_skew_finish_compensation(&ClkSkewULCtrl);
}


VOID clk_skew_inform_dl_leads_samples (U32 samples)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    ClkSkewDLCtrl.AccumulatedSamples += samples;
    hal_nvic_restore_interrupt_mask(mask);
}

VOID clk_skew_inform_dl_lags_samples (U32 samples)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    ClkSkewDLCtrl.AccumulatedSamples -= samples;
    hal_nvic_restore_interrupt_mask(mask);
}

S16 clk_skew_get_dl_comp_bytes (VOID)
{
    return ClkSkewDLCtrl.CompensatedSamples*ClkSkewDLCtrl.BytesPerSample;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 S16 clk_skew_check_dl_status (VOID)
{
    return clk_skew_check_status(&ClkSkewDLCtrl, CLK_SKEW_DL);
}

VOID clk_skew_finish_dl_compensation (VOID)
{
    clk_skew_finish_compensation(&ClkSkewDLCtrl);
}
#ifdef MTK_BT_HFP_SPE_ALG_V2
ATTR_TEXT_IN_IRAM S16 clk_skew_process (skew_ctrl_t_ptr  skew_ctrl_ptr, U8* inbuf, U8* outbuf, S16 compensate_bytes,U16 skew_points)
{
    U16  in_len_in_byte, ou_len_in_byte;
    S16  process_complete;
    U16  skew_comp_inc,skew_comp_dec,skew_comp_pass;
    
    if(skew_points == C_P125_Mode){
        skew_comp_inc = C_Skew_Inc_p125;
        skew_comp_dec = C_Skew_Dec_p125;
        skew_comp_pass = C_Skew_Pass;
    }else if(skew_points == C_P25_Mode){
        skew_comp_inc = C_Skew_Inc_p25;
        skew_comp_dec = C_Skew_Dec_p25;
        skew_comp_pass = C_Skew_Pass;
    }else if(skew_points == C_1_Mode){
        skew_comp_inc = C_Skew_Inc;
        skew_comp_dec = C_Skew_Dec;
        skew_comp_pass = C_Skew_Pass;
    }
    
    if (compensate_bytes > 0) {
        process_complete = skew_ctrl_process(skew_ctrl_ptr, (U8*)inbuf, &in_len_in_byte, (U8*)outbuf, &ou_len_in_byte, skew_comp_inc);
    } else if (compensate_bytes < 0) {
        process_complete = skew_ctrl_process(skew_ctrl_ptr, (U8*)inbuf, &in_len_in_byte, (U8*)outbuf, &ou_len_in_byte, skew_comp_dec);
    } else {
        process_complete = skew_ctrl_process(skew_ctrl_ptr, (U8*)inbuf, &in_len_in_byte, (U8*)outbuf, &ou_len_in_byte, skew_comp_pass);
    }
    
/*    #ifdef DSP_CLK_SKEW_DEBUG_LOG
    if(compensate_bytes){
        DSP_MW_LOG_I("[CLK_SKEW]compensate_bytes:%d,process_complete:%d",2,compensate_bytes,process_complete);
    }
    #endif*/
    
    if ( (S16)(ou_len_in_byte - in_len_in_byte) != compensate_bytes && !process_complete) {
        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] DspFw bytes mismatch with DspAlg bytes: %d, %d",2,(S16)(ou_len_in_byte - in_len_in_byte), compensate_bytes);
        #endif
    }
    return process_complete;
}
#else
ATTR_TEXT_IN_IRAM VOID clk_skew_process (skew_ctrl_t_ptr  skew_ctrl_ptr, U8* inbuf, U8* outbuf, S16 compensate_bytes)
{
    U16  in_len_in_byte, ou_len_in_byte;

    if (compensate_bytes > 0) {
        skew_ctrl_process(skew_ctrl_ptr, (U8*)inbuf, &in_len_in_byte, (U8*)outbuf, &ou_len_in_byte, C_Skew_Inc);
    } else if (compensate_bytes < 0) {
        skew_ctrl_process(skew_ctrl_ptr, (U8*)inbuf, &in_len_in_byte, (U8*)outbuf, &ou_len_in_byte, C_Skew_Dec);
    } else {
        skew_ctrl_process(skew_ctrl_ptr, (U8*)inbuf, &in_len_in_byte, (U8*)outbuf, &ou_len_in_byte, C_Skew_Pass);
    }

    if ( (S16)(ou_len_in_byte - in_len_in_byte) != compensate_bytes) {
        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] DspFw bytes mismatch with DspAlg bytes: %d, %d", 2,(S16)(ou_len_in_byte - in_len_in_byte), compensate_bytes);
        #endif
    }
}
#endif


VOID Clock_Skew_Alg_Init (skew_ctrl_t_ptr skew_ctrl_ptr, U16 bits, U16 skew_io_mode)
{
    #ifndef MTK_BT_HFP_SPE_ALG_V2
    U16 channel         = 1;            // 1(mono), 2(stereo)
    #endif
    //U16 framsize_num    = 1;            // skew_buf_len = framsize * framsize_num
    U16 order           = C_Flp_Ord_5;  // C_Flp_Ord_1(with 0 sample delay), C_Flp_Ord_3(with 1 sample delay), C_Flp_Ord_5(with 2 sample delay)
    #if 0
    U16 bits            = 16;           // 16-bit or 32-bit
    U16 framsize        = 240*2;        // for mono   mode in byte size, frame = 240 samples*2(16-bit)*1(mono)
    U16 skew_io_mode    = C_Skew_Inp;   // C_Skew_Inp, input sample length = skew_buf_len +/- 1, output sample length = skew_buf_len
                                        // C_Skew_Oup, input sample length = skew_buf_len      , output sample length = skew_buf_len +/- 1
    #endif
    
    #ifdef MTK_BT_HFP_SPE_ALG_V2
    skew_ctrl_init(skew_ctrl_ptr, bits, skew_io_mode, order);
    #else
    skew_ctrl_init(skew_ctrl_ptr, channel, bits, skew_io_mode, order);
    #endif
}


VOID Clock_Skew_Alg_Esco_Init (skew_ctrl_t_ptr skew_ctrl_ptr, U16 bits, U16 skew_io_mode)
{
    #ifndef MTK_BT_HFP_SPE_ALG_V2
    U16 channel         = 1;            // 1(mono), 2(stereo)
    #endif
    //U16 framsize_num    = 1;            // skew_buf_len = framsize * framsize_num
    U16 order           = C_Flp_Ord_1;  // C_Flp_Ord_1(with 0 sample delay), C_Flp_Ord_3(with 1 sample delay), C_Flp_Ord_5(with 2 sample delay)
    #if 0
    U16 bits            = 16;           // 16-bit or 32-bit
    U16 framsize        = 240*2;        // for mono   mode in byte size, frame = 240 samples*2(16-bit)*1(mono)
    U16 skew_io_mode    = C_Skew_Inp;   // C_Skew_Inp, input sample length = skew_buf_len +/- 1, output sample length = skew_buf_len
                                        // C_Skew_Oup, input sample length = skew_buf_len      , output sample length = skew_buf_len +/- 1
    #endif

    #ifdef MTK_BT_HFP_SPE_ALG_V2
    skew_ctrl_init(skew_ctrl_ptr, bits, skew_io_mode, order);
    #else
    skew_ctrl_init(skew_ctrl_ptr, channel, bits, skew_io_mode, order);
    #endif
}

/*
 * Clock_Skew_UL_Para_Init
 *
 * This function is used to init UL clock skew parameters
 *
 */
VOID Clock_Skew_UL_Para_Init()
{
    ClkSkewULCtrl.AccumulatedSamples = 0;
    ClkSkewULCtrl.CompensatedSamples = 0;
    ClkSkewULCtrl.CompensatedPolarity = 0;
    ClkSkewULCtrl.IntrDownCnt = 0;
    ClkSkewULCtrl.BytesPerSample = 0; // Should be defined from para
    ClkSkewULCtrl.DbgAccBytes = 0;
    ClkSkewULCtrl.Initialized = TRUE;
    ClkSkewULCtrl.InterruptHandleCnt = 0;
    ClkSkewULCtrl.UL_Prev_Polarity = 0;
    ClkSkewULCtrl.UL_Pol_Change_Samples = 0;
    fgULRCDCPolling = FALSE;
    fgUL1stRCDC = TRUE;

#ifdef ENABLE_HWSRC_CLKSKEW
    ClkSkewULRCDCCtrl.first_offset_flag = TRUE;
    ClkSkewULRCDCCtrl.initial_offset_flag = TRUE;
    ClkSkewULRCDCCtrl.prephase = 0;
    ClkSkewULRCDCCtrl.n9_clk_prev = 0;
    ClkSkewULRCDCCtrl.n9_clk_next = 0;
    ClkSkewULRCDCCtrl.aud_clk_prev = 0;
    ClkSkewULRCDCCtrl.aud_clk_next = 0;
#ifdef MTK_BT_HFP_SPE_ALG_V2
    ClkSkewULRCDCCtrl.bt_clk_offset_prev = BTCLK_INVALID_CLK;
    ClkSkewULRCDCCtrl.bt_clk_offset_next = BTCLK_INVALID_CLK;
    ClkSkewULRCDCCtrl.bt_intra_slot_offset_prev = BTINTRA_INVALID_CLK;
    ClkSkewULRCDCCtrl.bt_intra_slot_offset_next = BTINTRA_INVALID_CLK;
#else
    ClkSkewULRCDCCtrl.bt_clk_offset_prev = 0;
    ClkSkewULRCDCCtrl.bt_clk_offset_next = 0;
    ClkSkewULRCDCCtrl.bt_intra_slot_offset_prev = 0;
    ClkSkewULRCDCCtrl.bt_intra_slot_offset_next = 0;
#endif 

#endif
}

/*
 * Clock_Skew_DL_Para_Init
 *
 * This function is used to init DL clock skew parameters
 *
 */
VOID Clock_Skew_DL_Para_Init()
{
    ClkSkewDLCtrl.AccumulatedSamples = 0;
    ClkSkewDLCtrl.CompensatedSamples = 0;
    ClkSkewDLCtrl.CompensatedPolarity = 0;
    ClkSkewDLCtrl.IntrDownCnt = 0;
    ClkSkewDLCtrl.BytesPerSample = 0; // Should be defined from para
    ClkSkewDLCtrl.FrameSize = 0;
    ClkSkewDLCtrl.DbgAccBytes = 0;
    ClkSkewDLCtrl.Initialized = TRUE;

    ClkSkewLeadsNo = 0;
    ClkSkewLagsNo = 0;

    ClkSkewPCDCCtrl.asi_cnt = 0;
    ClkSkewPCDCCtrl.deviation_in_ppm = 0;
    ClkSkewPCDCCtrl.debug_bytes = 0;

    ClkSkewRCDCCtrl.first_offset_flag = TRUE;
    ClkSkewRCDCCtrl.initial_offset_flag = TRUE;
    ClkSkewRCDCCtrl.prephase = 0;
    ClkSkewRCDCCtrl.n9_clk_prev = 0;
    ClkSkewRCDCCtrl.n9_clk_next = 0;
    ClkSkewRCDCCtrl.aud_clk_prev = 0;
    ClkSkewRCDCCtrl.aud_clk_next = 0;
#ifdef MTK_BT_HFP_SPE_ALG_V2
    ClkSkewRCDCCtrl.bt_clk_offset_prev = BTCLK_INVALID_CLK;
    ClkSkewRCDCCtrl.bt_clk_offset_next = BTCLK_INVALID_CLK;
    ClkSkewRCDCCtrl.bt_intra_slot_offset_prev = BTINTRA_INVALID_CLK;
    ClkSkewRCDCCtrl.bt_intra_slot_offset_next = BTINTRA_INVALID_CLK;
    clk_skew_isr_time_initial();
#else
    ClkSkewRCDCCtrl.bt_clk_offset_prev = 0;
    ClkSkewRCDCCtrl.bt_clk_offset_next = 0;
    ClkSkewRCDCCtrl.bt_intra_slot_offset_prev = 0;
    ClkSkewRCDCCtrl.bt_intra_slot_offset_next = 0;    
#endif

#ifdef ENABLE_HWSRC_CLKSKEW
    rcdc_hfp_dl_clk_idx = 0;
#endif
}

/*
 * stream_function_clock_skew_uplink_initialize
 *
 * This function is used to init memory space for clock skew UL function
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_clock_skew_uplink_initialize (void *para)
{
    skew_ctrl_t_ptr  skew_ctrl_ptr1 = (skew_ctrl_t_ptr)stream_function_get_working_buffer(para);
    skew_ctrl_t_ptr  skew_ctrl_ptr2 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE);
    skew_ctrl_t_ptr  skew_ctrl_ptr3 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*2);
#ifdef ENABLE_2A2D_TEST
    skew_ctrl_t_ptr  skew_ctrl_ptr4 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*3);
    skew_ctrl_t_ptr  skew_ctrl_ptr5 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*4);
#endif
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    stream_ptr->source->param.audio.enable_clk_skew = true;

    #ifdef DSP_CLK_SKEW_DEBUG_LOG
    DSP_MW_LOG_I("[CLK_SKEW] UL Init", 0);
    #endif
    U16 BytesPerSample = (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) ? 4 : 2;

    Clock_Skew_UL_Para_Init();
    ClkSkewULCtrl.BytesPerSample = BytesPerSample; // Should be defined from para

    Clock_Skew_Alg_Esco_Init(skew_ctrl_ptr1, ClkSkewULCtrl.BytesPerSample*8, C_Skew_Inp);
    Clock_Skew_Alg_Esco_Init(skew_ctrl_ptr2, ClkSkewULCtrl.BytesPerSample*8, C_Skew_Inp);
    Clock_Skew_Alg_Esco_Init(skew_ctrl_ptr3, ClkSkewULCtrl.BytesPerSample*8, C_Skew_Inp);
#ifdef ENABLE_2A2D_TEST
    Clock_Skew_Alg_Esco_Init(skew_ctrl_ptr4, ClkSkewULCtrl.BytesPerSample*8, C_Skew_Inp);
    Clock_Skew_Alg_Esco_Init(skew_ctrl_ptr5, ClkSkewULCtrl.BytesPerSample*8, C_Skew_Inp);
#endif

    return FALSE;
}


/**
 * stream_function_clock_skew_downlink_initialize
 *
 * This function is used to init memory space for clock skew DL function
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_clock_skew_downlink_initialize (void *para)
{
    #ifdef DSP_CLK_SKEW_DEBUG_LOG
    DSP_MW_LOG_I("[CLK_SKEW] DL Init", 0);
    #endif

    U16 BytesPerSample = (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) ? 4 : 2;

    Clock_Skew_DL_Para_Init();
    ClkSkewDLCtrl.BytesPerSample = BytesPerSample; // Should be defined from para

    lt_clk_skew_set_asi_threshold(0x7FFFFFF);
    pcdc_asi_threshold = 0x7FFFFFFF;

    return FALSE;
}


/**
 * stream_function_clock_skew_uplink_process
 *
 * Clock skew compensation main process for UL audio
 *
 *
 * @para : Default parameter of callback function
 *
 */
#ifdef MTK_BT_HFP_SPE_ALG_V2
 ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_clock_skew_uplink_process (void *para)
{
    S16* Buf1 = (S16*)stream_function_get_1st_inout_buffer(para);
    S16* Buf2 = (S16*)stream_function_get_2nd_inout_buffer(para);
    S16* Buf3 = (S16*)stream_function_get_3rd_inout_buffer(para);
#ifdef ENABLE_2A2D_TEST
    S16* Buf4 = (S16*)stream_function_get_4th_inout_buffer(para);
    S16* Buf5 = (S16*)stream_function_get_5th_inout_buffer(para);
#endif
    U8 Channels = stream_function_get_channel_number(para);
    S16 FrameSize = (S16)stream_function_get_output_size(para);
    S16 CompensateBytes;
    skew_ctrl_t_ptr  skew_ctrl_ptr1 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*0);
    skew_ctrl_t_ptr  skew_ctrl_ptr2 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*1);
    skew_ctrl_t_ptr  skew_ctrl_ptr3 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*2);
#ifdef ENABLE_2A2D_TEST
    skew_ctrl_t_ptr  skew_ctrl_ptr4 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*3);
    skew_ctrl_t_ptr  skew_ctrl_ptr5 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*4);
#endif
    U8* ClkSkewInBuf = stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*Channels;
#ifdef ENABLE_HWSRC_CLKSKEW
    CLK_SKEW_FS_t fs = clk_skew_fs_converter((stream_samplerate_t)(U8)stream_function_get_samplingrate(para));
#endif
    S16 process_complete;
    if (FrameSize == 0) {
        return FALSE;
    }

    #ifdef ENABLE_HWSRC_CLKSKEW
    clk_skew_get_ul_accumulated_bytes_v2(fs);
    #endif

    CompensateBytes = clk_skew_get_ul_comp_bytes();

    if (Channels >= 1) {
        memcpy(ClkSkewInBuf, Buf1, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr1, FrameSize+CompensateBytes);
        process_complete = clk_skew_process(skew_ctrl_ptr1, (U8*)ClkSkewInBuf, (U8*)Buf1, CompensateBytes,C_1_Mode);
    }

    if (Channels >= 2) {
        memcpy(ClkSkewInBuf, Buf2, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr2, FrameSize+CompensateBytes);
        process_complete = clk_skew_process(skew_ctrl_ptr2, (U8*)ClkSkewInBuf, (U8*)Buf2, CompensateBytes,C_1_Mode);
    }

    if (Channels >= 3) {
        memcpy(ClkSkewInBuf, Buf3, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr3, FrameSize+CompensateBytes);
        process_complete = clk_skew_process(skew_ctrl_ptr3, (U8*)ClkSkewInBuf, (U8*)Buf3, CompensateBytes,C_1_Mode);
    }

#ifdef ENABLE_2A2D_TEST
    if (Channels >= 4) {
        memcpy(ClkSkewInBuf, Buf4, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr4, FrameSize+CompensateBytes);
        process_complete = clk_skew_process(skew_ctrl_ptr4, (U8*)ClkSkewInBuf, (U8*)Buf4, CompensateBytes,C_1_Mode);
    }

    if (Channels >= 5) {
        memcpy(ClkSkewInBuf, Buf5, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr5, FrameSize+CompensateBytes);
        process_complete = clk_skew_process(skew_ctrl_ptr5, (U8*)ClkSkewInBuf, (U8*)Buf5, CompensateBytes,C_1_Mode);
    }
#endif

    if (CompensateBytes && !process_complete) {
        stream_function_modify_output_size(para, (U16)(FrameSize + CompensateBytes));

        ClkSkewULCtrl.DbgAccBytes += CompensateBytes;

        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] UL Skew Compensation Bytes:%d", 1, ClkSkewULCtrl.DbgAccBytes);
        #endif

        clk_skew_finish_ul_compensation();
    }

    return FALSE;
}
#else
 ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_clock_skew_uplink_process (void *para)
{
    S16* Buf1 = (S16*)stream_function_get_1st_inout_buffer(para);
    S16* Buf2 = (S16*)stream_function_get_2nd_inout_buffer(para);
    S16* Buf3 = (S16*)stream_function_get_3rd_inout_buffer(para);
#ifdef ENABLE_2A2D_TEST
    S16* Buf4 = (S16*)stream_function_get_4th_inout_buffer(para);
    S16* Buf5 = (S16*)stream_function_get_5th_inout_buffer(para);
#endif
    U8 Channels = stream_function_get_channel_number(para);
    S16 FrameSize = (S16)stream_function_get_output_size(para);
    S16 CompensateBytes;
    skew_ctrl_t_ptr  skew_ctrl_ptr1 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*0);
    skew_ctrl_t_ptr  skew_ctrl_ptr2 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*1);
    skew_ctrl_t_ptr  skew_ctrl_ptr3 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*2);
#ifdef ENABLE_2A2D_TEST
    skew_ctrl_t_ptr  skew_ctrl_ptr4 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*3);
    skew_ctrl_t_ptr  skew_ctrl_ptr5 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*4);
#endif
    U8* ClkSkewInBuf = stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*Channels;
#ifdef ENABLE_HWSRC_CLKSKEW
    CLK_SKEW_FS_t fs = clk_skew_fs_converter((stream_samplerate_t)(U8)stream_function_get_samplingrate(para));
#endif

    if (FrameSize == 0) {
        return FALSE;
    }

    #ifndef ENABLE_HWSRC_CLKSKEW
    clk_skew_get_ul_accumulated_bytes();
    #else
    if(ClkSkewMode_g == CLK_SKEW_V2) {
        clk_skew_get_ul_accumulated_bytes_v2(fs);
    } else {
        clk_skew_get_ul_accumulated_bytes();
    }
    #endif
    CompensateBytes = clk_skew_get_ul_comp_bytes();

    if (Channels >= 1) {
        memcpy(ClkSkewInBuf, Buf1, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr1, FrameSize+CompensateBytes);
        clk_skew_process(skew_ctrl_ptr1, (U8*)ClkSkewInBuf, (U8*)Buf1, CompensateBytes);
    }

    if (Channels >= 2) {
        memcpy(ClkSkewInBuf, Buf2, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr2, FrameSize+CompensateBytes);
        clk_skew_process(skew_ctrl_ptr2, (U8*)ClkSkewInBuf, (U8*)Buf2, CompensateBytes);
    }

    if (Channels >= 3) {
        memcpy(ClkSkewInBuf, Buf3, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr3, FrameSize+CompensateBytes);
        clk_skew_process(skew_ctrl_ptr3, (U8*)ClkSkewInBuf, (U8*)Buf3, CompensateBytes);
    }

#ifdef ENABLE_2A2D_TEST
    if (Channels >= 4) {
        memcpy(ClkSkewInBuf, Buf4, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr4, FrameSize+CompensateBytes);
        clk_skew_process(skew_ctrl_ptr4, (U8*)ClkSkewInBuf, (U8*)Buf4, CompensateBytes);
    }

    if (Channels >= 5) {
        memcpy(ClkSkewInBuf, Buf5, FrameSize);
        skew_ctrl_set_input_framesize(skew_ctrl_ptr5, FrameSize+CompensateBytes);
        clk_skew_process(skew_ctrl_ptr5, (U8*)ClkSkewInBuf, (U8*)Buf5, CompensateBytes);
    }
#endif

    if (CompensateBytes) {
        stream_function_modify_output_size(para, (U16)(FrameSize + CompensateBytes));

        ClkSkewULCtrl.DbgAccBytes += CompensateBytes;

        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] UL Skew Compensation Bytes:%d", 1, ClkSkewULCtrl.DbgAccBytes);
        #endif

        clk_skew_finish_ul_compensation();
    }

    return FALSE;
}
#endif




/**
 * stream_function_clock_skew_downlink_process
 *
 * Clock skew compensation main process for DL audio
 *
 *
 * @para : Default parameter of callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_clock_skew_downlink_process (void *para)
{
    return Clock_Skew_DL_Main_Process(para, CLK_SKEW_DL_A2DP);
}


/**
 * stream_function_clock_skew_hfp_downlink_process
 *
 * Clock skew compensation main process for eSCO DL audio
 *
 *
 * @para : Default parameter of callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_clock_skew_hfp_downlink_process (void *para)
{
    return Clock_Skew_DL_Main_Process(para, CLK_SKEW_DL_ESCO);
}


#ifdef AIR_BT_CODEC_BLE_ENABLED
/**
 * stream_function_clock_skew_ble_music_downlink_process
 *
 * Clock skew compensation main process for BLE Music
 *
 *
 * @para : Default parameter of callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_clock_skew_ble_music_downlink_process (void *para)
{
    return Clock_Skew_DL_Main_Process(para, CLK_SKEW_DL_BLE_MUSIC);
}
#endif

/**
 * Clock_Skew_DL_Main_Process
 *
 * Clock skew compensation main process for DL audio
 *
 *
 * @para : Default parameter of callback function
 *  type : Scneario type
 *
 */
#ifdef MTK_BT_HFP_SPE_ALG_V2
ATTR_TEXT_IN_IRAM_LEVEL_2 void clk_skew_buffer_shift(S8* Des, S8* Src, S16 FrameSize, U8 BytesPerSample, S8 channel, bool reset)
{
    static S32 clk_skew_que_32[2];//{L_last_sample, R_last_sample}
    static S16 clk_skew_que_16[2];//{L_last_sample, R_last_sample}
    if (reset) {
        memset(clk_skew_que_16, 0, sizeof(clk_skew_que_16));
        memset(clk_skew_que_32, 0, sizeof(clk_skew_que_32));
        DSP_MW_LOG_I("[CLK_SKEW] clk_skew_buffer_shift!!", 0);
        return;
    }
    if (Des == NULL || Src == NULL) {
        DSP_MW_LOG_E("[CLK_SKEW] clk_skew_buffer_shift NULL ptr!!", 0);
        //assert(0);
        return;
    }

    S8* clk_skew_que_ptr = NULL;
    if (BytesPerSample == 2) {
        clk_skew_que_ptr = &clk_skew_que_16[channel];
    } else { //BytesPerSample == 4
        clk_skew_que_ptr = &clk_skew_que_32[channel];
    }
    memcpy(Des, clk_skew_que_ptr, BytesPerSample);
    memcpy(Des + BytesPerSample, Src, FrameSize);
    memcpy(clk_skew_que_ptr, Src + FrameSize - BytesPerSample, BytesPerSample);
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool Clock_Skew_DL_Main_Process (void *para, CLK_SKEW_DL_TYPE_t type)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    #ifdef ENABLE_HWSRC_CLKSKEW
    U32 fs_in = stream_ptr->sink->param.audio.src_rate;
    U32 fs_out = stream_ptr->sink->param.audio.rate;
    #endif
    S8* Buf1 = (S8*)stream_function_get_1st_inout_buffer(para);
    S8* Buf2 = (S8*)stream_function_get_2nd_inout_buffer(para);
    S16 FrameSize = (S16)stream_function_get_output_size(para);
    S16 Channels = (S16)stream_function_get_channel_number(para);
    CLK_SKEW_FS_t fs = clk_skew_fs_converter((stream_samplerate_t)(U8)stream_function_get_samplingrate(para));
    S16 CompensateBytes = 0,LTCS_CompensateBytes = 0;
    skew_ctrl_t_ptr  skew_ctrl_ptr1 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*0);
    skew_ctrl_t_ptr  skew_ctrl_ptr2 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*1);
    skew_ctrl_t_ptr  skew_ctrl_ptr3 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*2);
    skew_ctrl_t_ptr  skew_ctrl_ptr4 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*3);
    U8* ClkSkewInBuf = (U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*4;
    U8 BytesPerSample;
    S16  process_complete;

    BytesPerSample = (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) ? 4 : 2;


    if (FrameSize == 0) {
        return FALSE;
    }  else if ((FrameSize != ClkSkewDLCtrl.FrameSize) || (ClkSkewDLCtrl.BytesPerSample != BytesPerSample)) {
        if (type == CLK_SKEW_DL_A2DP) {
            Clock_Skew_Alg_Init(skew_ctrl_ptr1, BytesPerSample*8, C_Skew_Oup);
            Clock_Skew_Alg_Init(skew_ctrl_ptr2, BytesPerSample*8, C_Skew_Oup);
            Clock_Skew_Alg_Init(skew_ctrl_ptr3, BytesPerSample*8, C_Skew_Oup);
            Clock_Skew_Alg_Init(skew_ctrl_ptr4, BytesPerSample*8, C_Skew_Oup);
            clk_skew_buffer_shift(ClkSkewInBuf, Buf1, FrameSize, BytesPerSample, 0, TRUE);
        } else if ((type == CLK_SKEW_DL_ESCO) || (type == CLK_SKEW_DL_BLE_MUSIC)) {
            DSP_MW_LOG_I("[CLK_SKEW] DL Init!!", 0);
            Clock_Skew_Alg_Esco_Init(skew_ctrl_ptr1, BytesPerSample*8, C_Skew_Oup);
            clk_skew_buffer_shift(ClkSkewInBuf, Buf1, FrameSize, BytesPerSample, 0, TRUE);
        }

        ClkSkewDLCtrl.FrameSize = FrameSize;
        ClkSkewDLCtrl.BytesPerSample = BytesPerSample;

        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] DL Frame Size Init: %d", 1, ClkSkewDLCtrl.FrameSize);
        #endif
    }

    /* Calculate accumulated bytes or modify hwsrc step*/
    #ifndef MTK_BT_HFP_SPE_ALG_V2
    clk_skew_get_dl_accumulated_bytes(fs);
    #else /*ifdef MTK_BT_HFP_SPE_ALG_V2*/
        #ifndef ENABLE_HWSRC_CLKSKEW
            if (type == CLK_SKEW_DL_A2DP) {
                clk_skew_get_dl_accumulated_bytes_v2(fs);
            } else if (type == CLK_SKEW_DL_ESCO) {
               clk_skew_get_dl_accumulated_bytes_hfp(fs);
               clk_skew_isr_time_accumulated_drift(fs);
            }
        #else /*ifdef ENABLE_HWSRC_CLKSKEW*/
            if(ClkSkewMode_g == CLK_SKEW_V1){
                if (type == CLK_SKEW_DL_A2DP) {
                    clk_skew_get_dl_accumulated_bytes_v2(fs);
                } else if (type == CLK_SKEW_DL_ESCO) {
                    clk_skew_get_dl_accumulated_bytes_hfp(fs);
                    clk_skew_isr_time_accumulated_drift(fs);
                }else if(type == CLK_SKEW_DL_BLE_MUSIC) {
                    clk_skew_get_dl_accumulated_bytes_v2(fs);
                }
            } else if(ClkSkewMode_g == CLK_SKEW_V2){
                clk_skew_modify_hwsrc_step(fs, type, FrameSize, BytesPerSample, fs_in, fs_out);
                if(type == CLK_SKEW_DL_ESCO) {
                    clk_skew_get_dl_accumulated_bytes_from_ul(fs_in, fs_out);
                    if(clk_skew_get_dl_comp_bytes() != 0) {
                        clk_skew_finish_dl_compensation();
                    }
                } else if(type == CLK_SKEW_DL_BLE_MUSIC) {
#ifdef AIR_BT_CODEC_BLE_ENABLED
                if(stream_ptr->source->param.n9ble.dl_only == true)
#endif
                {
                    clk_skew_get_LE_accumulated_bytes_v2(fs_in);
                }
                    clk_skew_get_dl_accumulated_bytes_from_ul(fs_in, fs_out);
                    ClkSkewULCtrl.AccumulatedSamples = 0;
                    if(clk_skew_get_dl_comp_bytes() != 0) {
                        clk_skew_finish_dl_compensation();
                    }
                }
            } else {
                DSP_MW_LOG_W("[CLK_SKEW] No this clk skew type: %d", 1, ClkSkewMode_g);
            }
        #endif /*ENABLE_HWSRC_CLKSKEW*/
    #endif /*MTK_BT_HFP_SPE_ALG_V2*/

#ifdef ENABLE_HWSRC_CLKSKEW
if(ClkSkewMode_g == CLK_SKEW_V1)
#endif
{
    U16 skew_mode;
    #ifdef MTK_CELT_DEC_ENABLE
    if(stream_ptr->callback.FeatureTablePtr->ProcessEntry == stream_codec_decoder_celt_process){
        skew_mode = C_P125_Mode;
    }else{
        skew_mode = C_1_Mode;
    }
    #else
    skew_mode = C_1_Mode;
    #endif
    
    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)Buf1,(FrameSize),SOURCE_IN2);
    #endif
    
    //DSP_MW_LOG_W("[CLK_SKEW] ClkSkewDLCtrl.ClkSkewMode:%d", 1, ClkSkewMode_g);
    CompensateBytes = clk_skew_get_dl_comp_bytes();
    clk_skew_buffer_shift(ClkSkewInBuf, Buf1, FrameSize, BytesPerSample, 0,FALSE);
    //memcpy(ClkSkewInBuf, Buf1, FrameSize);
    skew_ctrl_set_input_framesize(skew_ctrl_ptr1, FrameSize);
    process_complete = clk_skew_process(skew_ctrl_ptr1, (U8*)ClkSkewInBuf, (U8*)Buf1, CompensateBytes,skew_mode);
    if (Channels == 2) {
        if (type == CLK_SKEW_DL_A2DP) {
            clk_skew_buffer_shift(ClkSkewInBuf, Buf2, FrameSize, BytesPerSample, 1,FALSE);
            //memcpy(ClkSkewInBuf, Buf2, FrameSize);
            skew_ctrl_set_input_framesize(skew_ctrl_ptr2, FrameSize);
            process_complete = clk_skew_process(skew_ctrl_ptr2, (U8*)ClkSkewInBuf, (U8*)Buf2, CompensateBytes,skew_mode);
        } else if (type == CLK_SKEW_DL_ESCO) {
        #ifndef MTK_DEQ_ENABLE
            memcpy(Buf2, Buf1, (FrameSize + CompensateBytes));
        #endif
        }
    }

    if (CompensateBytes && !process_complete) {
        stream_function_modify_output_size(para, (U16)(FrameSize + CompensateBytes));
        ClkSkewDLCtrl.DbgAccBytes += CompensateBytes;
        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] DL Skew Compensation Bytes:%d,out:%d",2, ClkSkewDLCtrl.DbgAccBytes,(FrameSize + CompensateBytes));
        #endif
        clk_skew_finish_dl_compensation();
    }else{
        CompensateBytes = 0;
    }
    
    /* PCDC module in DSP */
    if (type == CLK_SKEW_DL_A2DP) {
        ClkSkewPCDCCtrl.deviation_in_ppm += (FrameSize/BytesPerSample)*lt_clk_skew_get_dsp_drift_value();
        ClkSkewPCDCCtrl.asi_cnt += (FrameSize/BytesPerSample);

        #ifdef MTK_PCDC_TIMEOUT_ENABLE
        int asi_threshold_fixfs = lt_clk_skew_timeout_check(pcdc_asi_threshold, ClkSkewPCDCCtrl.asi_cnt, (int)fs);
        ClkSkewPCDCCtrl.asi_cnt -= asi_threshold_fixfs;
        #endif

        if (ClkSkewPCDCCtrl.asi_cnt >= lt_clk_skew_get_asi_threshold((int)fs)) {
            ClkSkewPCDCCtrl.asi_cnt -= lt_clk_skew_get_asi_threshold((int)fs);
            DSP_MW_LOG_I("[LTCS] Change of drift, current ASI:%d, ASI diff:%d", 2, ClkSkewPCDCCtrl.asi_cnt, lt_clk_skew_get_asi_threshold((int)fs));
            ClkSkewPCDCCtrl.deviation_in_ppm -= ClkSkewPCDCCtrl.asi_cnt * lt_clk_skew_get_dsp_drift_value(); // recover old drift value
            lt_clk_skew_update_actual_dsp_drift();
            ClkSkewPCDCCtrl.deviation_in_ppm += ClkSkewPCDCCtrl.asi_cnt * lt_clk_skew_get_dsp_drift_value(); // apply new drift value
            lt_clk_skew_set_asi_threshold(0x7FFFFFFF);
            pcdc_asi_threshold = 0x7FFFFFFF;
        } 

        if (ClkSkewPCDCCtrl.deviation_in_ppm > 1000000) {
            ClkSkewPCDCCtrl.deviation_in_ppm -= 1000000;
            LTCS_CompensateBytes += BytesPerSample;
            ClkSkewPCDCCtrl.debug_bytes += BytesPerSample;
        }

        if (ClkSkewPCDCCtrl.deviation_in_ppm < -1000000) {
            ClkSkewPCDCCtrl.deviation_in_ppm += 1000000;
            LTCS_CompensateBytes -= BytesPerSample;
            ClkSkewPCDCCtrl.debug_bytes -= BytesPerSample;
        }

        memcpy(ClkSkewInBuf, Buf1, (FrameSize + CompensateBytes));
        skew_ctrl_set_input_framesize(skew_ctrl_ptr3, (FrameSize + CompensateBytes));
        process_complete = clk_skew_process(skew_ctrl_ptr3, (U8*)ClkSkewInBuf, (U8*)Buf1, LTCS_CompensateBytes,skew_mode);

        if (Channels == 2) {
            memcpy(ClkSkewInBuf, Buf2, (FrameSize + CompensateBytes));
            skew_ctrl_set_input_framesize(skew_ctrl_ptr4, (FrameSize + CompensateBytes));
            process_complete = clk_skew_process(skew_ctrl_ptr4, (U8*)ClkSkewInBuf, (U8*)Buf2, LTCS_CompensateBytes,skew_mode);
        }
        
        if(LTCS_CompensateBytes){        
            if (LTCS_CompensateBytes && !process_complete) {
                #ifdef DSP_CLK_SKEW_DEBUG_LOG
                DSP_MW_LOG_I("[LTCS][DSP] Compensation Bytes:%d", 1, ClkSkewPCDCCtrl.debug_bytes);
                #endif
            }else{
                LTCS_CompensateBytes = 0;
            }

            stream_function_modify_output_size(para, (U16)(FrameSize + CompensateBytes + LTCS_CompensateBytes));
        }
    }

}
    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)Buf1,(FrameSize + CompensateBytes + LTCS_CompensateBytes),SOURCE_IN1);
    #endif
    return FALSE;
}
#else
ATTR_TEXT_IN_IRAM_LEVEL_2 bool Clock_Skew_DL_Main_Process (void *para, CLK_SKEW_DL_TYPE_t type)
{
    #ifdef ENABLE_HWSRC_CLKSKEW
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    U32 fs_in = stream_ptr->sink->param.audio.src_rate;
    U32 fs_out = stream_ptr->sink->param.audio.rate;
    #endif
    S8* Buf1 = (S8*)stream_function_get_1st_inout_buffer(para);
    S8* Buf2 = (S8*)stream_function_get_2nd_inout_buffer(para);
    S16 FrameSize = (S16)stream_function_get_output_size(para);
    S16 Channels = (S16)stream_function_get_channel_number(para);
    CLK_SKEW_FS_t fs = clk_skew_fs_converter((stream_samplerate_t)(U8)stream_function_get_samplingrate(para));
    S16 CompensateBytes = 0;
    skew_ctrl_t_ptr  skew_ctrl_ptr1 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*0);
    skew_ctrl_t_ptr  skew_ctrl_ptr2 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*1);
    skew_ctrl_t_ptr  skew_ctrl_ptr3 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*2);
    skew_ctrl_t_ptr  skew_ctrl_ptr4 = (skew_ctrl_t_ptr)((U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*3);
    U8* ClkSkewInBuf = (U8*)stream_function_get_working_buffer(para) + DSP_CLK_SKEW_MEMSIZE*4;
    U8 BytesPerSample;

    BytesPerSample = (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) ? 4 : 2;


    if (FrameSize == 0) {
        return FALSE;
    }  else if ((FrameSize != ClkSkewDLCtrl.FrameSize) || (ClkSkewDLCtrl.BytesPerSample != BytesPerSample)) {

        if (type == CLK_SKEW_DL_A2DP) {
            Clock_Skew_Alg_Init(skew_ctrl_ptr1, BytesPerSample*8, C_Skew_Oup);
            Clock_Skew_Alg_Init(skew_ctrl_ptr2, BytesPerSample*8, C_Skew_Oup);
            Clock_Skew_Alg_Init(skew_ctrl_ptr3, BytesPerSample*8, C_Skew_Oup);
            Clock_Skew_Alg_Init(skew_ctrl_ptr4, BytesPerSample*8, C_Skew_Oup);
        } else if ((type == CLK_SKEW_DL_ESCO) || (type == CLK_SKEW_DL_BLE_MUSIC)) {
            DSP_MW_LOG_I("[CLK_SKEW] DL Init!!", 0);
            Clock_Skew_Alg_Esco_Init(skew_ctrl_ptr1, BytesPerSample*8, C_Skew_Oup);
        }

        ClkSkewDLCtrl.FrameSize = FrameSize;
        ClkSkewDLCtrl.BytesPerSample = BytesPerSample;

        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] DL Frame Size Init: %d", 1, ClkSkewDLCtrl.FrameSize);
        #endif
    }

    /* Calculate accumulated bytes or modify hwsrc step*/
    #ifndef MTK_BT_HFP_SPE_ALG_V2
    clk_skew_get_dl_accumulated_bytes(fs);
    #else /*ifdef MTK_BT_HFP_SPE_ALG_V2*/
        #ifndef ENABLE_HWSRC_CLKSKEW
            if (type == CLK_SKEW_DL_A2DP) {
                clk_skew_get_dl_accumulated_bytes_v2(fs);
            } else if (type == CLK_SKEW_DL_ESCO) {
               clk_skew_get_dl_accumulated_bytes_hfp(fs);
            }
        #else /*ifdef ENABLE_HWSRC_CLKSKEW*/
            if(ClkSkewMode_g == CLK_SKEW_V1){
                if (type == CLK_SKEW_DL_A2DP) {
                    clk_skew_get_dl_accumulated_bytes_v2(fs);
                } else if (type == CLK_SKEW_DL_ESCO) {
                    clk_skew_get_dl_accumulated_bytes_hfp(fs);
                } else if(type == CLK_SKEW_DL_BLE_MUSIC) {
                    clk_skew_get_dl_accumulated_bytes_v2(fs);
                }
            } else if(ClkSkewMode_g == CLK_SKEW_V2){
                clk_skew_modify_hwsrc_step(fs, type, FrameSize, BytesPerSample, fs_in, fs_out);
                if(type == CLK_SKEW_DL_ESCO) {
                    clk_skew_get_dl_accumulated_bytes_from_ul(fs_in, fs_out);
                    if(clk_skew_get_dl_comp_bytes() != 0) {
                        clk_skew_finish_dl_compensation();
                    }
                } else if(type == CLK_SKEW_DL_BLE_MUSIC) {
#ifdef AIR_BT_CODEC_BLE_ENABLED
                    if(stream_ptr->source->param.n9ble.dl_only == true)
#endif
                    {
                        clk_skew_get_LE_accumulated_bytes_v2(fs_in);
                    }
                    clk_skew_get_dl_accumulated_bytes_from_ul(fs_in, fs_out);
                    ClkSkewULCtrl.AccumulatedSamples = 0;
                    if(clk_skew_get_dl_comp_bytes() != 0) {
                        clk_skew_finish_dl_compensation();
                    }
                }
            } else {
                DSP_MW_LOG_W("[CLK_SKEW] No this clk skew type: %d", 1, ClkSkewMode_g);
            }
        #endif /*ENABLE_HWSRC_CLKSKEW*/
    #endif /*MTK_BT_HFP_SPE_ALG_V2*/


#ifdef ENABLE_HWSRC_CLKSKEW
if(ClkSkewMode_g == CLK_SKEW_V1)
#endif
{
    //DSP_MW_LOG_W("[CLK_SKEW] ClkSkewDLCtrl.ClkSkewMode:%d", 1, ClkSkewMode_g);
    CompensateBytes = clk_skew_get_dl_comp_bytes();
    memcpy(ClkSkewInBuf, Buf1, FrameSize);
    skew_ctrl_set_input_framesize(skew_ctrl_ptr1, FrameSize);
    clk_skew_process(skew_ctrl_ptr1, (U8*)ClkSkewInBuf, (U8*)Buf1, CompensateBytes);
    if (Channels == 2) {
        if (type == CLK_SKEW_DL_A2DP) {
            memcpy(ClkSkewInBuf, Buf2, FrameSize);
            skew_ctrl_set_input_framesize(skew_ctrl_ptr2, FrameSize);
            clk_skew_process(skew_ctrl_ptr2, (U8*)ClkSkewInBuf, (U8*)Buf2, CompensateBytes);
        } else if (type == CLK_SKEW_DL_ESCO) {
        #ifndef MTK_DEQ_ENABLE
            memcpy(Buf2, Buf1, (FrameSize + CompensateBytes));
        #endif
        }
    }

    if (CompensateBytes) {
        stream_function_modify_output_size(para, (U16)(FrameSize + CompensateBytes));
        ClkSkewDLCtrl.DbgAccBytes += CompensateBytes;
        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] DL Skew Compensation Bytes:%d", 1, ClkSkewDLCtrl.DbgAccBytes);
        #endif
        clk_skew_finish_dl_compensation();
    }

    /* PCDC module in DSP */
    if (type == CLK_SKEW_DL_A2DP) {
        ClkSkewPCDCCtrl.deviation_in_ppm += (FrameSize/BytesPerSample)*lt_clk_skew_get_dsp_drift_value();
        ClkSkewPCDCCtrl.asi_cnt += (FrameSize/BytesPerSample);

        #ifdef MTK_PCDC_TIMEOUT_ENABLE
        int asi_threshold_fixfs = lt_clk_skew_timeout_check(pcdc_asi_threshold, ClkSkewPCDCCtrl.asi_cnt, (int)fs);
        ClkSkewPCDCCtrl.asi_cnt -= asi_threshold_fixfs;
        #endif

        if (ClkSkewPCDCCtrl.asi_cnt >= lt_clk_skew_get_asi_threshold((int)fs)) {
            ClkSkewPCDCCtrl.asi_cnt -= lt_clk_skew_get_asi_threshold((int)fs);
            DSP_MW_LOG_I("[LTCS] Change of drift, current ASI:%d, ASI diff:%d", 2, ClkSkewPCDCCtrl.asi_cnt, lt_clk_skew_get_asi_threshold((int)fs));
            ClkSkewPCDCCtrl.deviation_in_ppm -= ClkSkewPCDCCtrl.asi_cnt * lt_clk_skew_get_dsp_drift_value(); // recover old drift value
            lt_clk_skew_update_actual_dsp_drift();
            ClkSkewPCDCCtrl.deviation_in_ppm += ClkSkewPCDCCtrl.asi_cnt * lt_clk_skew_get_dsp_drift_value(); // apply new drift value
            lt_clk_skew_set_asi_threshold(0x7FFFFFFF);
            pcdc_asi_threshold = 0x7FFFFFFF;
        }

        S16 LTCS_CompensateBytes = 0;

        if (ClkSkewPCDCCtrl.deviation_in_ppm > 1000000) {
            ClkSkewPCDCCtrl.deviation_in_ppm -= 1000000;
            LTCS_CompensateBytes += BytesPerSample;
            ClkSkewPCDCCtrl.debug_bytes += BytesPerSample;
        }

        if (ClkSkewPCDCCtrl.deviation_in_ppm < -1000000) {
            ClkSkewPCDCCtrl.deviation_in_ppm += 1000000;
            LTCS_CompensateBytes -= BytesPerSample;
            ClkSkewPCDCCtrl.debug_bytes -= BytesPerSample;
        }

        memcpy(ClkSkewInBuf, Buf1, (FrameSize + CompensateBytes));
        skew_ctrl_set_input_framesize(skew_ctrl_ptr3, (FrameSize + CompensateBytes));
        clk_skew_process(skew_ctrl_ptr3, (U8*)ClkSkewInBuf, (U8*)Buf1, LTCS_CompensateBytes);

        if (Channels == 2) {
            memcpy(ClkSkewInBuf, Buf2, (FrameSize + CompensateBytes));
            skew_ctrl_set_input_framesize(skew_ctrl_ptr4, (FrameSize + CompensateBytes));
            clk_skew_process(skew_ctrl_ptr4, (U8*)ClkSkewInBuf, (U8*)Buf2, LTCS_CompensateBytes);
        }

        if (LTCS_CompensateBytes) {
            #ifdef DSP_CLK_SKEW_DEBUG_LOG
            DSP_MW_LOG_I("[LTCS][DSP] Compensation Bytes:%d", 1, ClkSkewPCDCCtrl.debug_bytes);
            #endif
        }

        stream_function_modify_output_size(para, (U16)(FrameSize + CompensateBytes + LTCS_CompensateBytes));
    }

}

    return FALSE;
}
#endif



#ifdef ENABLE_HWSRC_CLKSKEW
/**
 * HWSRC_Clock_Skew_Main_Process
 *
 * HWSRC Clock skew compensation main process
 *
 *
 * @para : Default parameter of callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool HWSRC_Clock_Skew_Main_Process (void *para, CLK_SKEW_DL_TYPE_t type)
{
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    U32 fs_in                         = stream_ptr->sink->param.audio.src_rate;
    U32 fs_out                        = stream_ptr->sink->param.audio.rate;
    CLK_SKEW_FS_t fs                  = clk_skew_fs_converter((stream_samplerate_t)(U8)stream_function_get_samplingrate(para));
    S16 FrameSize                     = (S16)stream_function_get_output_size(para);
    U8 BytesPerSample                 = (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) ? 4 : 2;

    if (FrameSize == 0) {
        return FALSE;
    }  else if ((FrameSize != ClkSkewDLCtrl.FrameSize) || (ClkSkewDLCtrl.BytesPerSample != BytesPerSample)) {
        ClkSkewDLCtrl.FrameSize = FrameSize;
        ClkSkewDLCtrl.BytesPerSample = BytesPerSample;
        DSP_MW_LOG_I("[CLK_SKEW] HWSRC clksew Init, Frame Size: %d, BytesPerSample: %d", 2, ClkSkewDLCtrl.FrameSize, ClkSkewDLCtrl.BytesPerSample);
    }

    clk_skew_modify_hwsrc_step(fs, type, FrameSize, BytesPerSample, fs_in, fs_out);

    return FALSE;
}
#endif


#ifdef PRELOADER_ENABLE
bool Clock_Skew_Open  (void *para)
{
    #ifdef DSP_CLK_SKEW_DEBUG_LOG
    DSP_MW_LOG_I("[PIC] Clock Skew Open", 0);
    #endif
    UNUSED(para);
    return TRUE;
}

bool Clock_Skew_Close (void *para)
{
    #ifdef DSP_CLK_SKEW_DEBUG_LOG
    DSP_MW_LOG_I("[PIC] Clock Skew Close", 0);
    #endif
    UNUSED(para);
    return TRUE;
}
#endif

#ifdef ENABLE_HWSRC_CLKSKEW

void pcdc_asi_count_add(U32 add_amount)
{
    U32 asi_old = ClkSkewPCDCCtrl.asi_cnt;
    ClkSkewPCDCCtrl.asi_cnt += add_amount;
    DSP_MW_LOG_I("[ASRC] asi add, asi_old:0x%x, asi_new:0x%x, add_amount:0x%x", 3, asi_old, ClkSkewPCDCCtrl.asi_cnt, add_amount);
}


/**
 * pcdc_asi_threshold_counter
 *
 * Accumulate the PCDC deviation and asi cnt for HWSRC irq hanlder
 *
 *
 * @para : sample_size: the process sample size of HWSRC (unit: sample)
 *         fs : output sampling rate
 *
 */
void pcdc_asi_threshold_counter(U32 sample_size, int fs)
{
    extern uint32_t sample_counter;
    ClkSkewPCDCCtrl.deviation_in_ppm += sample_size*lt_clk_skew_get_dsp_drift_value();
    ClkSkewPCDCCtrl.asi_cnt += sample_size;
    ClkSkewPCDCCtrl.asi_cnt = ClkSkewPCDCCtrl.asi_cnt&0x3FFFFFF; /*26bit*/
    //DSP_MW_LOG_I("[ASRC][LTCS] ASI:0x%x, ClkSkewPCDCCtrl.deviation_in_ppm:%d, sample_size:%d*drift:%d ", 4, ClkSkewPCDCCtrl.asi_cnt, ClkSkewPCDCCtrl.deviation_in_ppm, sample_size, lt_clk_skew_get_dsp_drift_value());
    if (ClkSkewPCDCCtrl.asi_cnt >= lt_clk_skew_get_asi_threshold(fs)) {
        DSP_MW_LOG_I("[ASRC][LTCS] Change of drift, current ASI:0x%x, ASI diff:0x%x, sample_counter:0x%x", 3, ClkSkewPCDCCtrl.asi_cnt, lt_clk_skew_get_asi_threshold(fs), sample_counter);
        ClkSkewPCDCCtrl.asi_cnt -= lt_clk_skew_get_asi_threshold(fs);
        ClkSkewPCDCCtrl.deviation_in_ppm -= ClkSkewPCDCCtrl.asi_cnt * lt_clk_skew_get_dsp_drift_value(); // recover old drift value, for the late IF pkt
        lt_clk_skew_update_actual_dsp_drift();
        ClkSkewPCDCCtrl.deviation_in_ppm += ClkSkewPCDCCtrl.asi_cnt * lt_clk_skew_get_dsp_drift_value(); // apply new drift value
        lt_clk_skew_set_asi_threshold(0x7FFFFFFF);
        sample_counter= 0;
    }
}
#endif

