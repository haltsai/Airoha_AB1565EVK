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

#include "at_command_audio_ata_test.h"
#include "hal_audio_internal.h"
#include <string.h>

#include "bt_sink_srv_ami.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_dvfs_internal.h"
#include "Audio_FFT.h"
#ifdef MTK_AUDIO_DUMP_ENABLE
#include "audio_dump.h"
#endif
#include <math.h>

#define SMT_DROP_CNT 55
#define SMT_SAVE 56
#define SMT_UL_CNT_LIMIT 60
#define REF_0DB 104857601

#define LOGMSGIDE(fmt,arg...)   LOG_MSGID_E(atcmd_aud, "ATCMD_AUD: "fmt,##arg)
#define LOGMSGIDW(fmt,arg...)   LOG_MSGID_W(atcmd_aud, "ATCMD_AUD: "fmt,##arg)
#define LOGMSGIDI(fmt,arg...)   LOG_MSGID_I(atcmd_aud ,"ATCMD_AUD: "fmt,##arg)


#if defined(__GNUC__)
//#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#if 1

//#if defined(HAL_AUDIO_TEST_ENABLE)
#if 1

#define SPM_BASE (0xA2120000)

static bool audio_smt_flag = false;
void audio_test_hp_hw_sine_(uint8_t chennel)
{
    //Digital part
    *(volatile uint32_t *)0xA2030B20 = 0x000001FC;   //XO_PDN_CLRD0
    hal_gpt_delay_us(250);
  //  if (!audio_smt_flag) { // disable HW sine when smt test
        *(volatile uint32_t *)0x70000000 = 0x00000000;   //AUDIO_TOP_CON0
        *(volatile uint32_t *)0x70000010 = 0x00000001;   //AFE_DAC_CON0
        *(volatile uint32_t *)0x70000124 = 0x60606001;   //AFE_ADDA_UL_DL_CON0
        if(chennel&SMT_CH_LEFT)
            *(volatile uint32_t *)0x700001F0 = 0x04AC2AC0;   //AFE_SINEGEN_CON0
        else if(chennel&SMT_CH_RIGHT)
            *(volatile uint32_t *)0x700001F0 = 0x04AC0AC2;   //AFE_SINEGEN_CON0
        *(volatile uint32_t *)0x700001DC = 0x00000024;   //AFE_SINEGEN_CON2
        //*(volatile uint32_t *)0x700001DC = 0x00000004;   //AFE_SINEGEN_CON2
  //  }
    *(volatile uint32_t *)0x70000260 = 0x00000000;   //AFE_ADDA_PREDIS_CON0
    *(volatile uint32_t *)0x70000264 = 0x00000000;   //AFE_ADDA_PREDIS_CON1
    *(volatile uint32_t *)0x70000D40 = 0x80000000;   //AFE_ADDA_PREDIS_CON2
    *(volatile uint32_t *)0x70000D44 = 0x00000000;   //AFE_ADDA_PREDIS_CON3
    *(volatile uint32_t *)0x70000C50 = 0x0700701E;   //AFE_ADDA_DL_SDM_DCCOMP_CON
    *(volatile uint32_t *)0x7000010C = 0x00000000;   //AFE_ADDA_DL_SRC2_CON1
    *(volatile uint32_t *)0x70000108 = 0x83001801;   //AFE_ADDA_DL_SRC2_CON0
    *(volatile uint32_t *)0x70000908 = 0x00000006;   //AFUNC_AUD_CON2
    *(volatile uint32_t *)0x70000900 = 0x0000CBA1;   //AFUNC_AUD_CON0
    *(volatile uint32_t *)0x70000908 = 0x00000003;   //AFUNC_AUD_CON2
    *(volatile uint32_t *)0x70000908 = 0x0000000B;   //AFUNC_AUD_CON2
    *(volatile uint32_t *)0x70000E6C = 0x00000050;   //AFE_ANA_GAIN_MUX
    hal_gpt_delay_us(50);

    //Analog part
    *(volatile uint32_t *)0xA2070224 = 0x00003020;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070224 = 0x00000020;   //AUDDEC_ANA_CON9
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA207022C = 0x00000004;   //AUDDEC_ANA_CON11
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA207022C = 0x00000183;   //AUDDEC_ANA_CON11
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA207022C = 0x00000182;   //AUDDEC_ANA_CON11
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070224 = 0x00000120;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070224 = 0x00000320;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070224 = 0x00000720;   //AUDDEC_ANA_CON9
*((volatile uint32_t*)(0xA2070224)) = 0x00003720;
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070228 = 0x00000010;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00000030;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00000070;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00000170;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00001170;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00009170;   //AUDDEC_ANA_CON10
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0x70000F50 = 0x00000000;   //ZCD_CON0
    *(volatile uint32_t *)0xA2070204 = 0x00003000;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA207020C = 0x00000100;   //AUDDEC_ANA_CON3
    *(volatile uint32_t *)0xA2070220 = 0x0000003F;   //AUDDEC_ANA_CON8
    *(volatile uint32_t *)0xA207021C = 0x00000103;   //AUDDEC_ANA_CON7
    *(volatile uint32_t *)0xA2070220 = 0x00000000;   //AUDDEC_ANA_CON8
    *(volatile uint32_t *)0xA2070228 = 0x00009173;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070214 = 0x00000000;   //AUDDEC_ANA_CON5
    if(chennel&SMT_CH_LEFT)
        *(volatile uint32_t *)0x70000F58 = 0x00002C0B;   //ZCD_CON2
    else if(chennel&SMT_CH_RIGHT)
        *(volatile uint32_t *)0x70000F58 = 0x00000B2C;
    *(volatile uint32_t *)0x70000EC4 = 0x0E021188;   //AFE_CLASSG_LPSRCH_CFG0
    *(volatile uint32_t *)0xA2070208 = 0x00000024;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA207020C = 0x00000300;     //AUDDEC_ANA_CON3
    *(volatile uint32_t *)0xA207020C = 0x00000300;     //AUDDEC_ANA_CON3
    *(volatile uint32_t *)0xA2070208 = 0x000001A4;     //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070208 = 0x000031A4;     //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070204 = 0x0000300C;     //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070204 = 0x000030CC;     //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070204 = 0x000030FC;     //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070208 = 0x00003DA4;     //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070208 = 0x0000FDA4;     //AUDDEC_ANA_CON2
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA2070208 = 0x0000CDA4;     //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070204 = 0x000030FF;     //AUDDEC_ANA_CON1
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0x70000EC4 = 0x0E0A1188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E121188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E1A1188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E221188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E2A1188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E321188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E3A1188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0xA207020C = 0x00000311;   //AUDDEC_ANA_CON3
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0xA207020C = 0x000003FF;   //AUDDEC_ANA_CON3
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0xA2070208 = 0x0000CC24;   //AUDDEC_ANA_CON2
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0x70000F58 = 0x00000AEB;   //ZCD_CON2
    *(volatile uint32_t *)0x70000F58 = 0x00000208;   //ZCD_CON2                               // Modify here to mute left or right
    *(volatile uint32_t *)0xA2070204 = 0x000030F3;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070208 = 0x0000C024;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070228 = 0x00009170;   //AUDDEC_ANA_CON10
    hal_gpt_delay_us(100);
    *((volatile uint32_t*)(0xA2070224)) = 0x00003721;
    *(volatile uint32_t *)0xA2070230 = 0x00000100;   //AUDDEC_ANA_CON12
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070200 = 0x00000020;   //AUDDEC_ANA_CON0
    *(volatile uint32_t *)0xA2070200 = 0x00000120;   //AUDDEC_ANA_CON0
    *(volatile uint32_t *)0xA2070200 = 0x0000012F;   //AUDDEC_ANA_CON0
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070204 = 0x000035F3;   //AUDDEC_ANA_CON1
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA207020C = 0x000001FF;   //AUDDEC_ANA_CON3
}

void audio_mtcmos_on(void)
{

    uint32_t audio_control;
    /* AUDIO MTCMOS ON */
    *((volatile uint32_t*)(SPM_BASE + 0x020C)) = 0x16;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x020C)) = 0x1E;
    hal_gpt_delay_us(1);
    audio_control = *((volatile uint32_t*)(SPM_BASE + 0x0700));
    *((volatile uint32_t*)(SPM_BASE + 0x0700)) = audio_control & 0xFF07;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x0700)) = audio_control & 0xFF03;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x0700)) = audio_control & 0xFF01;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x0700)) = audio_control & 0xFF00;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x0700)) = 0;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x020C)) = 0x0E;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x020C)) = 0x0C;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x020C)) = 0x1C;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x020C)) = 0x1D;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(SPM_BASE + 0x020C)) = 0x0D;
    hal_gpt_delay_us(1);
    *((volatile uint32_t*)(0xA2290020)) = 0; /* INFRA_CFG_MTCMOS2[0] : audiosys_prot_en */
    while((*((volatile uint32_t*)(0xA2290020))&0x100) != 0); /* INFRA_CFG_MTCMOS2[8] : audiosys_prot_rdy */
    *((volatile uint32_t*)(0xA2030B20)) = 0xFFFFFFFF; // clock CG all on
}


static void audio_turn_off_afe(void)
{
    //Digital part
    *(volatile uint32_t *)0x70000900 = 0x0000CB20;   //AFUNC_AUD_CON0
    *(volatile uint32_t *)0x70000908 = 0x00000000;   //AFUNC_AUD_CON2
    *(volatile uint32_t *)0x70000108 = 0x00000000;   //AFE_ADDA_DL_SRC2_CON0
    *(volatile uint32_t *)0x700001F0 = 0x00000000;   //AFE_SINEGEN_CON0
    *(volatile uint32_t *)0x70000124 = 0x60606000;   //AFE_ADDA_UL_DL_CON0
    *(volatile uint32_t *)0x70000010 = 0x00000000;   //AFE_DAC_CON0
    *(volatile uint32_t *)0x70000000 = 0x000CC000;   //AUDIO_TOP_CON0
    *(volatile uint32_t *)0xA2030B20 = 0x00000000;   //XO_PDN_CLRD0
    //Analog part
    *(volatile uint32_t *)0xA207020C = 0x000003FF;   //AUDDEC_ANA_CON3
    *(volatile uint32_t *)0xA2070228 = 0x00009173;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070204 = 0x000030F3;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070200 = 0x00000060;   //AUDDEC_ANA_CON0
    *(volatile uint32_t *)0xA2070230 = 0x00000000;   //AUDDEC_ANA_CON12
    *(volatile uint32_t *)0xA2070224 = 0x00000720;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070208 = 0x0000CC24;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070204 = 0x000030FF;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070204 = 0x000030FC;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070208 = 0x0000FDA4;   //AUDDEC_ANA_CON2
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA2070208 = 0x00003DA4;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070208 = 0x000031A4;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070204 = 0x000030CC;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070204 = 0x0000300C;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070204 = 0x00003000;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070208 = 0x0000C1A4;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070208 = 0x0000C024;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070228 = 0x00009170;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA207020C = 0x00000100;   //AUDDEC_ANA_CON3
    *(volatile uint32_t *)0x70000F50 = 0x00000000;   //ZCD_CON0
    *(volatile uint32_t *)0xA2070220 = 0x00008000;   //AUDDEC_ANA_CON8
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070228 = 0x00009160;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00009140;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00009100;   //AUDDEC_ANA_CON10
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070224 = 0x00000320;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070224 = 0x00000120;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070224 = 0x00000020;   //AUDDEC_ANA_CON9
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA207022C = 0x00000004;   //AUDDEC_ANA_CON11
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA2070224 = 0x00000030;   //AUDDEC_ANA_CON9
    hal_gpt_delay_us(100);
}

void audio_test_hp_hw_sine(uint8_t chennel)
{
    //Digital part
    *(volatile uint32_t *)0xA2030B20 = 0x000001FC;   //XO_PDN_CLRD0
    hal_gpt_delay_us(250);
    *(volatile uint32_t *)0x70000000 = 0x20004000;//AUDIO_TOP_CON0
    *(volatile uint32_t *)0x70000010 = 0x00000001;   //AFE_DAC_CON0
    *(volatile uint32_t *)0x700001F0 = 0x04AC2AC2;   //AFE_SINEGEN_CON0
    *(volatile uint32_t *)0x700001DC = 0x00000024;   //AFE_SINEGEN_CON2
    //*(volatile uint32_t *)0x700001DC = 0x00000004;   //AFE_SINEGEN_CON2
    *(volatile uint32_t *)0x70000260 = 0x00000000;   //AFE_ADDA_PREDIS_CON0
    *(volatile uint32_t *)0x70000264 = 0x00000000;   //AFE_ADDA_PREDIS_CON1
    *(volatile uint32_t *)0x70000D40 = 0x80000000;   //AFE_ADDA_PREDIS_CON2
    *(volatile uint32_t *)0x70000D44 = 0x00000000;   //AFE_ADDA_PREDIS_CON3
    *(volatile uint32_t *)0x70000C50 = 0x0700701E;   //AFE_ADDA_DL_SDM_DCCOMP_CON
    *(volatile uint32_t *)0x7000010C = 0x00000000;   //AFE_ADDA_DL_SRC2_CON1
    *(volatile uint32_t *)0x70000108 = 0x83001801;   //AFE_ADDA_DL_SRC2_CON0
    *(volatile uint32_t *)0x70000908 = 0x00000006;   //AFUNC_AUD_CON2
    *(volatile uint32_t *)0x70000900 = 0x0000CBA1;   //AFUNC_AUD_CON0
    *(volatile uint32_t *)0x70000908 = 0x00000003;   //AFUNC_AUD_CON2
    *(volatile uint32_t *)0x70000908 = 0x0000000B;   //AFUNC_AUD_CON2
    *(volatile uint32_t *)0x70000E6C = 0x00000050;   //AFE_ANA_GAIN_MUX
    hal_gpt_delay_us(50);
    //Analog part
    *(volatile uint32_t *)0xA2070224 = 0x00003020;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070224 = 0x00000020;   //AUDDEC_ANA_CON9
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA207022C = 0x00000004;   //AUDDEC_ANA_CON11
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA207022C = 0x00000183;   //AUDDEC_ANA_CON11
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA207022C = 0x00000182;   //AUDDEC_ANA_CON11
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070224 = 0x00000120;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070224 = 0x00000320;   //AUDDEC_ANA_CON9
    *(volatile uint32_t *)0xA2070224 = 0x00000720;   //AUDDEC_ANA_CON9
*((volatile uint32_t*)(0xA2070224)) = 0x00003720;
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070228 = 0x00000010;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00000030;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00000070;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00000170;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00001170;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070228 = 0x00009170;   //AUDDEC_ANA_CON10
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0x70000F50 = 0x00000000;   //ZCD_CON0
    *(volatile uint32_t *)0xA2070204 = 0x00003000;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA207020C = 0x00000100;   //AUDDEC_ANA_CON3
    *(volatile uint32_t *)0xA2070220 = 0x0000003F;   //AUDDEC_ANA_CON8
    *(volatile uint32_t *)0xA207021C = 0x00000103;   //AUDDEC_ANA_CON7
    *(volatile uint32_t *)0xA2070220 = 0x00000000;   //AUDDEC_ANA_CON8
    *(volatile uint32_t *)0xA2070228 = 0x00009173;   //AUDDEC_ANA_CON10
    *(volatile uint32_t *)0xA2070214 = 0x00000000;   //AUDDEC_ANA_CON5
    if(chennel&SMT_CH_LEFT)
        *(volatile uint32_t *)0x70000F58 = 0x00003E0B;   //ZCD_CON2
    else if(chennel&SMT_CH_RIGHT)
        *(volatile uint32_t *)0x70000F58 = 0x00000B3E;
    *(volatile uint32_t *)0x70000EC4 = 0x0E021188;   //AFE_CLASSG_LPSRCH_CFG0
    *(volatile uint32_t *)0xA2070208 = 0x00000024;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA207020C = 0x00000300;     //AUDDEC_ANA_CON3
    *(volatile uint32_t *)0xA207020C = 0x00000300;     //AUDDEC_ANA_CON3
    *(volatile uint32_t *)0xA2070208 = 0x000001A4;     //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070208 = 0x000031A4;     //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070204 = 0x0000300C;     //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070204 = 0x000030CC;     //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070204 = 0x000030FC;     //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070208 = 0x00003DA4;     //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070208 = 0x0000FDA4;     //AUDDEC_ANA_CON2
    hal_gpt_delay_us(50);
    *(volatile uint32_t *)0xA2070208 = 0x0000CDA4;     //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070204 = 0x000030FF;     //AUDDEC_ANA_CON1
    hal_gpt_delay_us(50);

    *(volatile uint32_t *)0x70000EC4 = 0x0E0A1188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E121188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E1A1188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E221188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E2A1188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E321188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0x70000EC4 = 0x0E3A1188;   //AFE_CLASSG_LPSRCH_CFG0
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0xA207020C = 0x00000311;   //AUDDEC_ANA_CON3
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0xA207020C = 0x000003FF;   //AUDDEC_ANA_CON3
    hal_gpt_delay_us(600);
    *(volatile uint32_t *)0xA2070208 = 0x0000CC24;   //AUDDEC_ANA_CON2
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0x70000F58 = 0x00000AEB;   //ZCD_CON2
    *(volatile uint32_t *)0x70000F58 = 0x00000208;   //ZCD_CON2                               // Modify here to mute left or right
    *(volatile uint32_t *)0xA2070204 = 0x000030F3;   //AUDDEC_ANA_CON1
    *(volatile uint32_t *)0xA2070208 = 0x0000C024;   //AUDDEC_ANA_CON2
    *(volatile uint32_t *)0xA2070228 = 0x00009170;   //AUDDEC_ANA_CON10
    hal_gpt_delay_us(100);
*((volatile uint32_t*)(0xA2070224)) = 0x00003721;
    *(volatile uint32_t *)0xA2070230 = 0x00000100;   //AUDDEC_ANA_CON12
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070200 = 0x00000020;   //AUDDEC_ANA_CON0
    *(volatile uint32_t *)0xA2070200 = 0x00000120;   //AUDDEC_ANA_CON0
    *(volatile uint32_t *)0xA2070200 = 0x0000012F;   //AUDDEC_ANA_CON0
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA2070204 = 0x000035F3;   //AUDDEC_ANA_CON1
    hal_gpt_delay_us(100);
    *(volatile uint32_t *)0xA207020C = 0x000001FF;   //AUDDEC_ANA_CON3

}


void hal_audio_init_stream_buf(fft_buf_t *fft_bufs){
    if(!fft_bufs)
        return ;
    fft_bufs->cpyIdx = &fft_bufs->bitstream_buf[0];
    memset(fft_bufs->cpyIdx,0,FFT_BUFFER_SIZE << 1);
}

void audio_smt_test_pure_on_off(bool enable,smt_ch chennel)
{

    if (enable) {
        uint32_t interconnection = 0;
        audio_smt_flag = true;
        audio_mtcmos_on();

        interconnection = *(volatile uint32_t *)0x70000438;
        *(volatile uint32_t *)0x70000438 = interconnection | 0x0000100; // I8->O8
        interconnection = *(volatile uint32_t *)0x70000440;
        *(volatile uint32_t *)0x70000440 = interconnection | 0x0000200; // I9->O9
        audio_test_hp_hw_sine_(chennel);
#ifndef AIR_ATA_TEST_ENABLE
        pmu_power_enable_6388(PMU_BUCK_VAUD18,PMU_ON);
#endif

    } else {
          audio_turn_off_afe();
          *(volatile uint32_t*)0x70000114 = 0x00000000;   //AFE_ADDA_UL_SRC_CON0
          audio_smt_flag = false;
    }
}

uint32_t pass_through_test(pass_through_test_mic_type mic, pass_through_test_mic_side side, pass_through_test_freq freq, pPTT_u4Freq_Mag_data result)
{
    int i = 0;
    uint32_t Atcmd_value = *((volatile uint32_t*)(0xA2120B04));
    static fft_buf_t *fft_bufs = NULL;

    if (fft_bufs == NULL) {
        LOGMSGIDI("ATA loopback create fft_bufs\r\n", 0);
        fft_bufs = (fft_buf_t *)pvPortMalloc(sizeof(fft_buf_t));
    }
    if (fft_bufs != NULL) {
        hal_audio_init_stream_buf(fft_bufs);

        if(mic < PTT_DMIC) { //AMIC
            ami_set_audio_device(STREAM_IN, AU_DSP_VOICE, HAL_AUDIO_DEVICE_MAIN_MIC_DUAL, HAL_AUDIO_INTERFACE_1, NOT_REWRITE);
            if(mic == PTT_AMIC_ACC) {
                *((volatile uint32_t*)(0xA2120B04)) |= 0x10;
            }
        }
        else{//DMIC
            ami_set_audio_device(STREAM_IN, AU_DSP_VOICE, HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL, HAL_AUDIO_INTERFACE_1, NOT_REWRITE);
        }

        if(side == PTT_L) {
            ami_set_audio_channel(AUDIO_CHANNEL_L, AUDIO_CHANNEL_NONE, NOT_REWRITE);
        }
        else {
            ami_set_audio_channel(AUDIO_CHANNEL_R, AUDIO_CHANNEL_NONE, NOT_REWRITE);
        }

        //KTONE_DL_ON
        {
            dsp2mcu_audio_msg_t open_msg = MSG_MCU2DSP_PROMPT_OPEN;
            dsp2mcu_audio_msg_t start_msg = MSG_MCU2DSP_PROMPT_START;
            audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PROMPT;
            void *p_param_share;
/*
#if (PRODUCT_VERSION == 1552)
            dvfs_lock_control("AUDIO",DVFS_78M_SPEED, DVFS_LOCK);
#endif
#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
            dvfs_lock_control("AUDIO", HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_LOCK);//frequency is risen to 0.8V for bus hi-res out
#endif
*/
            hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, true);
            hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true);
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_OUTPUT_DEVICE_VOLUME, 0, 0, false);
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 0, 0, false);
            //hal_audio_set_stream_in_volume(3000, 0);
            mcu2dsp_open_param_t open_param;
            open_param.param.stream_in    = STREAM_IN_VP;
            open_param.param.stream_out = STREAM_OUT_AFE;
            open_param.stream_in_param.playback.bit_type = HAL_AUDIO_BITS_PER_SAMPLING_16;
            open_param.stream_in_param.playback.sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            open_param.stream_in_param.playback.channel_number = HAL_AUDIO_MONO;
            open_param.stream_in_param.playback.codec_type = 0;
            open_param.stream_in_param.playback.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(msg_type);
            hal_audio_reset_share_info( open_param.stream_in_param.playback.p_share_info );
            open_param.stream_in_param.playback.p_share_info->sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            open_param.stream_in_param.playback.p_share_info->bBufferIsFull = 1;
            open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL;
            open_param.stream_out_param.afe.stream_channel    = HAL_AUDIO_DIRECT;
            open_param.stream_out_param.afe.memory            = HAL_AUDIO_MEM2;
            open_param.stream_out_param.afe.format            = AFE_PCM_FORMAT_S16_LE;
            open_param.stream_out_param.afe.stream_out_sampling_rate    = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_16KHZ);
            open_param.stream_out_param.afe.sampling_rate     = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_16KHZ);
            open_param.stream_out_param.afe.irq_period        = 10;
            open_param.stream_out_param.afe.frame_size        = 512;
            open_param.stream_out_param.afe.frame_number    = 4;
            open_param.stream_out_param.afe.hw_gain         = true;
            p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), msg_type);
            hal_audio_dsp_controller_send_message(open_msg, AUDIO_DSP_CODEC_TYPE_PCM, (uint32_t)p_param_share, true);
            mcu2dsp_start_param_t start_param;
            start_param.param.stream_in     = STREAM_IN_VP;
            start_param.param.stream_out    = STREAM_OUT_AFE;
            start_param.stream_out_param.afe.aws_flag          = false;
            start_param.stream_out_param.afe.aws_sync_request = true;
            start_param.stream_out_param.afe.aws_sync_time      = 1000;
            p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), msg_type);
            hal_audio_dsp_controller_send_message(start_msg, 0, (uint32_t)p_param_share, true);
            if(freq < PTT_MUTE){
                *(volatile uint32_t *)0x700001F0 = 0x044E04E0 | (freq) | (freq << 12);
            }
            else{
                *(volatile uint32_t *)0x700001F0 = 0x044E04E0 | (freq);
            }
            *(volatile uint32_t *)0x700001DC = 0x00000024;
            LOGMSGIDI("[PASS_THRU_TEST]0x700001F0 = 0x%x\r\n", 1, *(volatile uint32_t *)0x700001F0);
        }
        hal_audio_start_stream_in(HAL_AUDIO_RECORD_VOICE);
        i=0;

        while (1) {
            if (i < (SMT_DROP_CNT)) {
                hal_audio_read_stream_in(fft_bufs->cpyIdx, FFT_BUFFER_SIZE); //drop
            } else if(i < SMT_SAVE) {
                fft_bufs->cpyIdx = fft_bufs->cpyIdx + (FFT_BUFFER_SIZE >> 1);
            } else if(i < SMT_UL_CNT_LIMIT) {
                if (HAL_AUDIO_STATUS_OK == hal_audio_read_stream_in(fft_bufs->cpyIdx, FFT_BUFFER_SIZE)){
                    break;
                }
            } else {
                break;
            }
            hal_gpt_delay_ms(5);
            i++;
        }

        hal_audio_stop_stream_in();
        //KTONE_DL_OFF;
        {
            dsp2mcu_audio_msg_t stop_msg = MSG_MCU2DSP_PROMPT_STOP;
            dsp2mcu_audio_msg_t close_msg = MSG_MCU2DSP_PROMPT_CLOSE;
            hal_audio_dsp_controller_send_message(stop_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
            hal_audio_dsp_controller_send_message(close_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
/*
#if (PRODUCT_VERSION == 1552)
            dvfs_lock_control("AUDIO",DVFS_78M_SPEED, DVFS_UNLOCK);
#endif
#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
            dvfs_lock_control("AUDIO", HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);//frequency set back from hwsrc hi-res 0.8V out
#endif
*/
            hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, false);
            hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
        }
//        LOG_AUDIO_DUMP(fft_bufs->bitstream_buf, (sizeof(int16_t) * 256), freq);
        ApplyFFT256(fft_bufs->bitstream_buf, 0, &fft_bufs->u4Freq_data, &fft_bufs->u4Mag_data, 16000);
        *((volatile uint32_t*)(0xA2120B04)) = Atcmd_value;

        result->freq_data = fft_bufs->u4Freq_data;
        result->mag_data = fft_bufs->u4Mag_data;
        result->db_data = 10.0 * log10((double)(fft_bufs->u4Mag_data / (double)REF_0DB));
        LOGMSGIDI("[PASS_THRU_TEST]Freq=%d, Mag=%d, db=%d\r\n", 3, fft_bufs->u4Freq_data, fft_bufs->u4Mag_data, (int)round(result->db_data));
        return PTT_SUCCESS;
    }

    return PTT_FAIL;
}

#endif /* defined(HAL_AUDIO_TEST_ENABLE) */
#endif /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
#endif /*defined(__GNUC__)*/


#ifdef AIR_ATA_TEST_ENABLE
#include "hal_audio.h"
#include "hal_platform.h"
extern bool g_ata_running;
extern hal_audio_analog_mdoe_t g_ata_amic_mode;
extern HAL_DSP_PARA_AU_AFE_CTRL_t audio_nvdm_HW_config;
extern VOID hal_audio_ata_test_sinegen(uint32_t kHz, uint32_t path);

void ata_test_ktone_DL(bool enable){
    if(enable){ //DL on
        dsp2mcu_audio_msg_t open_msg = MSG_MCU2DSP_PROMPT_OPEN;
        dsp2mcu_audio_msg_t start_msg = MSG_MCU2DSP_PROMPT_START;
        audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PROMPT;
        void *p_param_share;

        hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, true);
        hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_OUTPUT_DEVICE_VOLUME, 0, 0, false);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 0, 0x00000FA0, false); //a = 0dB, d = 40dB

        #if 0
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 0, 0x00180018, false);
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 1, 0x04B004B0, false);
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 2, 0x04B004B0, false);
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 3, 0x04B004B0, false);
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 13, 0x00000708, false); //a = 0dB, d = 18dB
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 14, 0x04B004B0, false); //a = 12dB, d = 12dB
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 15, 0x04B004B0, false); //a = 12dB, d = 12dB
        #endif

        // Collect parameters
        mcu2dsp_open_param_t open_param;

        open_param.param.stream_in  = STREAM_IN_VP;
        open_param.param.stream_out = STREAM_OUT_AFE;

        open_param.stream_in_param.playback.bit_type = HAL_AUDIO_BITS_PER_SAMPLING_16;
        open_param.stream_in_param.playback.sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
        open_param.stream_in_param.playback.channel_number = HAL_AUDIO_MONO;
        open_param.stream_in_param.playback.codec_type = 0;
        open_param.stream_in_param.playback.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(msg_type);
        hal_audio_reset_share_info( open_param.stream_in_param.playback.p_share_info );
        open_param.stream_in_param.playback.p_share_info->bBufferIsFull = 0;

        hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);

        open_param.stream_out_param.afe.memory            = HAL_AUDIO_MEM2;
        open_param.stream_out_param.afe.format            = AFE_PCM_FORMAT_S16_LE;
        open_param.stream_out_param.afe.stream_out_sampling_rate    = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ);
        open_param.stream_out_param.afe.sampling_rate    = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ);open_param.stream_out_param.afe.irq_period      = 10;
        open_param.stream_out_param.afe.frame_size      = 512;
        open_param.stream_out_param.afe.frame_number    = 4;
        open_param.stream_out_param.afe.hw_gain         = true;

        #ifdef AIR_FIXED_SUB_DL_HIGH_RES_ENABLE
        hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PROMPT, open_param.stream_out_param.afe.audio_device, 96000, true);
        #else
        hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PROMPT, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, true);
        #endif

        p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), msg_type);
        hal_audio_dsp_controller_send_message(open_msg, AUDIO_DSP_CODEC_TYPE_PCM, (uint32_t)p_param_share, true);

        mcu2dsp_start_param_t start_param;
        start_param.param.stream_in     = STREAM_IN_VP;
        start_param.param.stream_out    = STREAM_OUT_AFE;
        start_param.stream_out_param.afe.aws_flag          = false;
        start_param.stream_out_param.afe.aws_sync_request = true;
        start_param.stream_out_param.afe.aws_sync_time      = 1000;
        p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), msg_type);
        hal_audio_dsp_controller_send_message(start_msg, 0, (uint32_t)p_param_share, true);

        //SineGen!!!!
        hal_audio_ata_test_sinegen(0, 0); //1kHz at DL
    }else{ //DL off
        dsp2mcu_audio_msg_t stop_msg = MSG_MCU2DSP_PROMPT_STOP;
        dsp2mcu_audio_msg_t close_msg = MSG_MCU2DSP_PROMPT_CLOSE;
        hal_audio_dsp_controller_send_message(stop_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
        hal_audio_dsp_controller_send_message(close_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
        hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, false);
        hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
    }
}

void ata_test_start_stream_in(bool enable){
    if(enable){
        g_ata_running = true;
        #if 0
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_VC;
            //vol_info.vol_info.vc_vol_info.dev_in = stream_out->audio_device;
            //vol_info.vol_info.vc_vol_info.lev_in = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
        #else
        hal_audio_set_stream_in_volume(3000, 0);
        #endif
        #endif

        hal_audio_start_stream_in(HAL_AUDIO_RECORD_VOICE);
        //hal_audio_ata_test_sinegen(0, 1); //1k sinegen test adc01
        //hal_audio_ata_test_sinegen(0, 2); //1k sinegen test adc23
        //hal_audio_ata_test_sinegen(0, 3); //1k sinegen test adc45

    }else{
        hal_audio_stop_stream_in();
        g_ata_running = false;
    }
}


//*move to driver layer*/
void ata_test_set_mic(hal_audio_device_t device, uint32_t adc_id){
    hal_audio_interface_t i2s_interface;
    if(device == HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL){ //DMIC
        switch(adc_id){
            case 0:
                i2s_interface = HAL_AUDIO_INTERFACE_1;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = DMIC_ADC0; //0x08;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = DMIC_ADC1; //0x09;
            case 1:
                i2s_interface = HAL_AUDIO_INTERFACE_1;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = DMIC_ADC1; //0x08;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = DMIC_ADC0; //0x09;
                //audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = HAL_AUDIO_DMIC_ANA_DMIC0; //0x08;
                //audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = HAL_AUDIO_DMIC_ANA_DMIC1; //0x09;
                break;
            case 2:
                i2s_interface = HAL_AUDIO_INTERFACE_2;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = DMIC_ADC2; //0x0A;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = DMIC_ADC3; //0x0B;
                break;
            case 3:
                i2s_interface = HAL_AUDIO_INTERFACE_2;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = DMIC_ADC3; //0x0A;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = DMIC_ADC2; //0x0B;
                break;
            case 4:
                i2s_interface = HAL_AUDIO_INTERFACE_3;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = DMIC_ADC4; //0x0C;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = DMIC_ADC5; //0x0D;
                break;
            case 5:
                i2s_interface = HAL_AUDIO_INTERFACE_3;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = DMIC_ADC5; //0x0C;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = DMIC_ADC4; //0x0D;
                break;
            default:
                break;
        }
    }else{ //device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL:  //AMIC
        switch(adc_id){
            case 0:
                i2s_interface = HAL_AUDIO_INTERFACE_1;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = AMIC_ADC0; //0x00;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = AMIC_ADC1; //0x01;
                break;
            case 1:
                i2s_interface = HAL_AUDIO_INTERFACE_1;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = AMIC_ADC1; //0x00;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = AMIC_ADC0; //0x01;
                break;
            case 2:
                i2s_interface = HAL_AUDIO_INTERFACE_2;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = AMIC_ADC2; //0x02;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = AMIC_ADC3; //0x03;
                break;
            case 3:
                i2s_interface = HAL_AUDIO_INTERFACE_2;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = AMIC_ADC3; //0x02;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = AMIC_ADC2; //0x03;
                break;
            case 4:
                i2s_interface = HAL_AUDIO_INTERFACE_3;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = AMIC_ADC4; //0x04;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = AMIC_ADC5; //0x05;
                break;
            case 5:
                i2s_interface = HAL_AUDIO_INTERFACE_3;
                ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, device, i2s_interface, NOT_REWRITE);
                audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = AMIC_ADC5; //0x04;
                audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = AMIC_ADC4; //0x05;
                break;
            default:
                break;
        }
    }
}
#endif
