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

#include "hal_clock_internal.h"
#ifdef HAL_CLOCK_MODULE_ENABLED
#include "hal_nvic_internal.h"
#include <assert.h>
#include <stdio.h>
#include "hal_gpt.h"
#include "hal_flash_sf.h"
#include "hal_esc_internal.h"
#include "hal.h"
#ifndef FPGA_ENV
ATTR_RWDATA_IN_TCM static cali_info_t osc_tune_tbl[TUNE_TBL_SIZE] = {{.scale = lposc_cali_scale, .src = AD_OSC_D3_CK,.cali_target = OSC_CLK_139M},
                                                                     {.scale = lposc_ft_scale,   .src = AD_OSC_CK,   .cali_target = OSC_CLK_416M}};
#endif
#define eof_div_tbl(tbl)        &((((clk_div_info*)tbl)->field).div_max)
#define BYTE_REG(instance)      ((volatile uint8_t *)&instance)
#define FT_SCALE                150
static void lposc_enable();
#if !defined(__EXT_BOOTLOADER__) || defined(__EXT_DA__)

static void lposc_disable();
ATTR_TEXT_IN_TCM static uint8_t clk_pll_on(clock_pll_id pll_id);
ATTR_TEXT_IN_TCM static uint8_t clk_pll_off(clock_pll_id pll_id);

ATTR_RWDATA_IN_TCM static clk_apll_freq apll_freq[NR_APLL] = {CLK_APLL_45M,CLK_APLL_49M};

ATTR_RWDATA_IN_TCM static clock_domain_t clock_domain = {
    .top_mux_info_list = {
        {.cur_sel_0 = NULL,
         .cur_sel_1 = NULL,
         .cur_sel_2 = NULL,
         .cur_sel_s = NULL,
         .sels = {{CLK_XO, CLK_DIV_NONE},     //CLK_SYS_SEL       0 : XO_CK,     26 MHz
                      {CLK_OSC, CLK_DIV_D2},       //             1 : OSC_D2_D8, 26 MHz
                      {CLK_OSC,CLK_DIV_NONE},      //             2 : OSC,       416 MHz
                      {CLK_OSC,CLK_DIV_D2},        //             3 : OSC_D2,    208 MHz
                      {CLK_OSC,CLK_DIV_D2},        //             4 : OSC_D2_D2, 104 MHz
                      {CLK_OSC,CLK_DIV_D5},        //             5 : OSC_D5,    83.2 MHz
                      {CLK_UPLL,CLK_DIV_D2},       //             6 : UPLL_D2_D2, 156 MHz
                      {CLK_UPLL,CLK_DIV_D3}}       //             7 : UPLL_D3,    104 MHz
        },
        {
         .sels = {{CLK_XO, CLK_DIV_NONE},     //CLK_DSP_SEL       0 : XO_CK,     26 MHz
                      {CLK_OSC, CLK_DIV_D2},       //             1 : OSC_D2_D8, 26 MHz
                      {CLK_OSC,CLK_DIV_NONE},      //             2 : OSC,       416 MHz
                      {CLK_OSC,CLK_DIV_D2},        //             3 : OSC_D2,    208 MHz
                      {CLK_OSC,CLK_DIV_D2},        //             4 : OSC_D2_D2, 104 MHz
                      {CLK_OSC,CLK_DIV_D5},        //             5 : OSC_D5,    83.2 MHz
                      {CLK_UPLL,CLK_DIV_D2},       //             6 : UPLL_D2_D2, 156 MHz
                      {CLK_UPLL,CLK_DIV_D3}}       //             7 : UPLL_D3,    104 MHz
        },
        {.sels = {{CLK_XO,  CLK_DIV_NONE},//CLK_SFC_SEL           0 : XO_CK,     26 MHz
                      {CLK_OSC, CLK_DIV_D2},       //             1 : OSC_D2_D8, 26 MHz
                      {CLK_OSC,CLK_DIV_D2},        //             2 : OSC_D2_D2, 104 MHz
                      {CLK_OSC,CLK_DIV_D5},        //             3 : OSC_D5,    83.2 MHz
                      {CLK_OSC,CLK_DIV_D3},        //             4 : OSC_D3_D2, 69.33 MHz
                      {CLK_OSC, CLK_DIV_D2},       //             5 : OSC_D2_D4, 52 MHz
                      {CLK_UPLL,CLK_DIV_D3},       //             6 : UPLL_D2_D2 156 MHz
                      {CLK_UPLL,CLK_DIV_D2}}       //             7 : UPLL_D3,   104 MHz
        },
        {.sels = {{CLK_XO,  CLK_DIV_NONE},//CLK_ESC_SEL           0 : XO_CK,     26 MHz
                      {CLK_OSC,CLK_DIV_D2},       //              1 : OSC_D2_D8, 26 MHz
                      {CLK_OSC,CLK_DIV_D2},        //             2 : OSC_D2_D2, 52 MHz
                      {CLK_OSC,CLK_DIV_D5},        //             3 : OSC_D5,    41.6 MHz
                      {CLK_OSC,CLK_DIV_D3},        //             4 : OSC_D3_D2, 34.67 MHz
                      {CLK_UPLL,CLK_DIV_D3},       //             5 : UPLL_D3_D2,52 MHz
                      {CLK_UPLL,CLK_DIV_D2},       //             6 : UPLL_D2_D2,39 MHz
                      {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO, CLK_DIV_NONE},    //CLK_SPIMST0_SEL    0 : XO_CK,     26 MHz
                     {CLK_OSC,CLK_DIV_D2},        //              1 : OSC_D2_D8, 26 MHz
                     {CLK_OSC,CLK_DIV_D2},         //             2 : OSC_D2_D2, 104 MHz
                     {CLK_OSC,CLK_DIV_D5},         //             3 : OSC_D5,    83.2 MHz
                     {CLK_OSC,CLK_DIV_D3},        //              4 : OSC_D3_D2, 69.33 MHz
                     {CLK_OSC,CLK_DIV_D2},        //              5 : OSC_D2_D4, 52 MHz
                     {CLK_UPLL,CLK_DIV_D3},        //             6 : UPLL_D3,   104 MHz
                     {CLK_UPLL,CLK_DIV_D3}}        //             7 : UPLL_D3_D2, 52 MHz
        },
        {.sels = {{CLK_XO, CLK_DIV_NONE},    //CLK_SDIOMST0_SEL   0 : XO_CK,     26 MHz
                     {CLK_OSC,CLK_DIV_D2},        //             1 : OSC_D2_D8, 26 MHz
                     {CLK_OSC,CLK_DIV_D2},         //             2 : OSC_D2_D2, 52 MHz
                     {CLK_OSC,CLK_DIV_D5},         //             3 : OSC_D5,    41.6 MHz
                     {CLK_OSC,CLK_DIV_D3},        //             4 : OSC_D3_D2, 34.67 MHz
                     {CLK_UPLL,CLK_DIV_D3},       //             5 : UPLL_D3_D2,52 MHz
                     {CLK_UPLL,CLK_DIV_D2},        //             6 : UPLL_D2_D4,39 MHz
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO, CLK_DIV_NONE},    //CLK_USB_SEL        0 : XO_CK,     26 MHz
                     {CLK_OSC,CLK_DIV_D2},        //              1 : OSC_D2_D8, 26 MHz
                     {CLK_UPLL,CLK_DIV_D5},         //            2 : UPLL_D5,   62.4 MHz
                     {CLK_OSC,CLK_DIV_D2},         //             3 : OSC_D2_D4, 52 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO,CLK_DIV_NONE},  //CLK_AUD_BUS_SEL       0 : XO_CK,     26 MHz
                     {CLK_OSC,CLK_DIV_D3},        //              1 : OSC_D3,    138.67 MHz
                     {CLK_OSC,CLK_DIV_D2},        //              2 : OSC_D2_D2  104 MHz
                     {CLK_OSC,CLK_DIV_D3},        //              3 : OSC_D3_D2, 69.33 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO,CLK_DIV_NONE},     //CLK_AUD_GPSRC_SEL  0 : XO_CK,     26 MHz
                     {CLK_OSC,CLK_DIV_D3},         //             1 : OSC_D3,    138.67 MHz
                     {CLK_OSC,CLK_DIV_D2},         //             2 : OSC_D2_D2, 104 MHz
                     {CLK_OSC,CLK_DIV_D2},         //             3 : OSC_D2_D4, 52 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO,CLK_DIV_NONE},      //CLK_AUD_ULCK_SEL  0 : XO_CK,     26 MHz
                     {CLK_OSC,CLK_DIV_D2},         //             1 : OSC_D2,    208 MHz
                     {CLK_OSC,CLK_DIV_D3},         //             2 : OSC_D3,    138.67 MHz
                     {CLK_OSC,CLK_DIV_D2},         //             3 : OSC_D2_D2, 104 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO,CLK_DIV_NONE},      //CLK_AUD_DLCK_SEL  0 : XO_CK,     26 MHz
                     {CLK_OSC,CLK_DIV_D3},       //               1 : OSC_D3,    138.67 MHz
                     {CLK_OSC,CLK_DIV_D2},       //               2 : OSC_D2_D2, 104 MHz
                     {CLK_OSC,CLK_DIV_D5},       //               3 : OSC_D5,    83.2 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO,CLK_DIV_NONE},      // CLK_26M_SEL      0 : XO_CK,     26 MHz
                     {CLK_OSC,CLK_DIV_D2},        //              1 : OSC_D2_D8, 26 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO, CLK_DIV_NONE},      //CLK_AUD_INTERFACE0_SEL 0 : XO_CK, 13 MHz
                     {CLK_OSC, CLK_DIV_D2},        //             1 : OSC_D2_D8,   13 MHz
                     {CLK_APLL1,CLK_DIV_NONE},      //            2 : APLL1_CK,    24.576 / 22.579 MHz
                     {CLK_APLL2,CLK_DIV_NONE},      //            3 : APLL2_CK,    24.576 / 22.579 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_XO,CLK_DIV_NONE},      //CLK_AUD_ENGINE_SEL 0 : XO_CK,     13 MHz
                     {CLK_OSC,CLK_DIV_D2},        //               1 : OSC_D2_D8, 13 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_APLL1,CLK_DIV_NONE},     //CLK_AUD_VOW_SEL     0 : XO_CK
                     {CLK_OSC,CLK_DIV_D2},      //                    1 : OSC_D2_D8, 26 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
        {.sels = {{CLK_APLL1,CLK_DIV_NONE},//                     0 : APLL1_CK,    24.576 / 22.579 MHz
                     {CLK_APLL2,CLK_DIV_NONE},  //                1 : APLL2_CK,    24.576 / 22.579 MHz
                     {CLK_XO,CLK_DIV_NONE},     //                2 : XO_CK,       26 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        }
    },
    .peri_mux_info_list = {
        {.cur_sel_0 = NULL,
         .sels = {{CLK_XO, CLK_DIV_NONE},     //CLK_PWM0_SEL      0 : XO_CK,     26 MHz
                     {CLK_OSC, CLK_DIV_D2},       //              1 : OSC_D2_D8, 26 MHz
                     {CLK_XO,CLK_DIV_NONE},       //              2 : RTC,       32 kHz
                     {CLK_OSC,CLK_DIV_D5},        //              3 : OSC_D5_D2, 41.6 MHz
                     {CLK_XO,CLK_DIV_NONE},
                     {CLK_XO,CLK_DIV_NONE}}
        },
    },

    .div_info ={
        .div_tbl = {
            {.field = {   // UPLL_SRC
                .div_low       = CLK_DIV_D2,
                .div_middle    = CLK_DIV_D3,
                .div_high      = CLK_DIV_D5,
                .div_max       = CLK_DIV_NONE}
            },
            {.field = {   // LPOSC_SRC
                .div_low =    CLK_DIV_D2,
                .div_middle = CLK_DIV_D3,
                .div_high =   CLK_DIV_D5,
                .div_max  =   CLK_DIV_NONE}
            }
        }
    }
};

uint8_t clock_set_pll_on(clock_pll_id pll_id){
    return 1;
}

uint8_t clock_set_pll_off(clock_pll_id pll_id){
    return 1;
}

void hal_clock_set_running_flags(uint32_t clock_cg_opt_set,bool on_off){}
void clock_dump_info(void){};

ATTR_RODATA_IN_TCM static const uint32_t clk_linkage_tbl[] = {
    HAL_CLOCK_CG_END,              //CLK_SYS_SEL
    HAL_CLOCK_CG_END,              //CLK_DSP_SEL
    HAL_CLOCK_CG_END,              //CLK_SFC_SEL
    HAL_CLOCK_CG_END,              //CLK_ESC_SEL
    HAL_CLOCK_CG_SPIMST0,          //CLK_SPIMST0_SEL
    HAL_CLOCK_CG_SDIOMST0,         //CLK_SDIOMST0_SEL
    HAL_CLOCK_CG_USB,              //CLK_USB_SEL
    HAL_CLOCK_CG_AUD_INTBUS,       //CLK_AUD_BUS_SEL
    HAL_CLOCK_CG_AUD_GPSRC,        //CLK_AUD_GPSRC_SEL
    HAL_CLOCK_CG_AUD_UPLINK,       //CLK_AUD_ULCK_SEL
    HAL_CLOCK_CG_AUD_DWLINK,       //CLK_AUD_DLCK_SEL
    HAL_CLOCK_CG_END,              //CLK_26M_SEL
    HAL_CLOCK_CG_AUD_INTF0,        //CLK_AUD_INTF0_SEL
    HAL_CLOCK_CG_END,              //CLK_AUD_ENGINE_SEL
    HAL_CLOCK_CG_END,              //CLK_VOW_SEL
    HAL_CLOCK_CG_END,              //CLK_MCLK_SEL
    HAL_CLOCK_CG_END,              //NR_MUXS
    HAL_CLOCK_CG_SPIMST1,          //CLK_SPIMST1_SEL 
    HAL_CLOCK_CG_SPIMST2,          //CLK_SPIMST2_SEL
    HAL_CLOCK_CG_SPISLV,           //CLK_SPISLV_SEL 
    HAL_CLOCK_CG_AUD_INTF1,        //CLK_AUD_INTF1_SEL 
};

ATTR_TEXT_IN_TCM clk_usr_status clock_get_apll1_status(){
    return APLL1_DOMAIN_USR_EXIST;
}
ATTR_TEXT_IN_TCM clk_usr_status clock_get_apll2_status(){
    return APLL2_DOMAIN_USR_EXIST;
}

ATTR_TEXT_IN_TCM clk_usr_status clock_get_upll_status(){
    return UPLL_DOMAIN_USR_EXIST;
}

ATTR_TEXT_IN_TCM clk_usr_status clock_get_lposc_status(){
    return LPOSC_DOMAIN_USR_EXIST;
}

ATTR_TEXT_IN_TCM static void apll1_enable(void){
    clk_pll_on(CLK_APLL1);
}
ATTR_TEXT_IN_TCM static void apll1_disable(void){
    clk_pll_off(CLK_APLL1);
}
ATTR_TEXT_IN_TCM static void apll2_enable(void){
    clk_pll_on(CLK_APLL2);
}
ATTR_TEXT_IN_TCM static void apll2_disable(void){
    clk_pll_off(CLK_APLL2);
}
ATTR_TEXT_IN_TCM static void upll_disable(void){
    clk_pll_off(CLK_UPLL);
}
ATTR_TEXT_IN_TCM static void upll_enable(void){
    clk_pll_on(CLK_UPLL);
}

ATTR_TEXT_IN_TCM uint8_t *div_memmem(uint8_t *tbl,uint8_t div){
    uint8_t *query = tbl;
    for(;query <= eof_div_tbl(tbl); query++){
        if(*query == div){
            return query;
        }else if(*query == CLK_DIV_RESERVED){
            break;
        }
    }
    return NULL;
}

ATTR_TEXT_IN_TCM hal_clock_status_t clk_topgen_input_directly(clock_pll_id src,mod_type type){
    uint32_t *domain_base = src == CLK_OSC ? (uint32_t *)&clock_domain.lposc_domain.field.free:
                                             (uint32_t *)&clock_domain.dcxo_domain.field.free;
    volatile uint8_t *p_src_cnt = (volatile uint8_t *)domain_base;
    if(src != CLK_OSC){
        switch(src){
        case CLK_APLL1:
            break;
        case CLK_APLL2:
            p_src_cnt+= 1;
            break;
        case CLK_XO:
            p_src_cnt+= 2;
            break;
        case CLK_UPLL:
            p_src_cnt+= 3;
            break;
        default:
            return HAL_CLOCK_STATUS_ERROR;
        }
    }
    if(type){
        (*p_src_cnt) ++;
    }else if(*p_src_cnt){
        (*p_src_cnt) --;
    }

    return HAL_CLOCK_STATUS_OK;
}

ATTR_TEXT_IN_TCM static void lposc_disable(){
    CLKSQ_CON_REG.RG_LPOSC_RDY = 0x0;
    //CLKSQ_CON_REG.RG_HFOSC_104M_EN = 0x0; //optional
    LPOSC_CON_REG.DA_HFOSC_EN = 0x0;
}

ATTR_TEXT_IN_TCM hal_clock_status_t clock_div_ctrl(pin_mux_t mux,bool on_off){

    hal_clock_status_t result = HAL_CLOCK_STATUS_OK;
    uint8_t shift = 0;
    volatile uint8_t *div_opt_reg_base = NULL;
    clock_div_ctrl_idx div_idx = NONE_DIV;
    clk_usr_status sta = exist;
    clk_src_handler src_hdr = {NULL,NULL};

    switch(mux.src){

    case CLK_APLL1:
        src_hdr = (clk_src_handler){clock_get_apll1_status,apll1_enable};
        sta = src_hdr.usr_sta();
        break;
    case CLK_APLL2:
        src_hdr = (clk_src_handler){clock_get_apll2_status,apll2_enable};
        sta = src_hdr.usr_sta();
        break;
    case CLK_UPLL:
        div_opt_reg_base = &CKSYS_CLK_DIV_REG.CLK_PLL1_D2_EN;
        src_hdr = (clk_src_handler){clock_get_upll_status,upll_enable};
        sta = src_hdr.usr_sta();
        div_idx = UPLL_DIV_IDX;
        break;
    case CLK_OSC:
        div_opt_reg_base = &CKSYS_CLK_DIV_REG.CLK_OSC1_D2_EN;
        src_hdr = (clk_src_handler){clock_get_lposc_status,lposc_enable};
        sta = src_hdr.usr_sta();
        div_idx = OSC_DIV_IDX;

        break;
    case CLK_XO:
        break;
    default :
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
    __IO uint8_t *DIV_PWER = NULL;
    __IO uint8_t *DIV_EN   = NULL;
    __IO uint8_t *p_res_mgr= NULL;
    if(div_idx < NONE_DIV){
        __IO uint8_t *div_group_base = BYTE_REG(clock_domain.div_info.div_tbl[div_idx]);
        uint8_t *p_div = div_memmem((uint8_t*)div_group_base,mux.div);
        if(p_div){
            shift = p_div - div_group_base;
            DIV_PWER = div_idx == OSC_DIV_IDX ? &(CKSYS_CLK_DIV_REG.CLK_OSC1_DIV_EN):
                                                &(CKSYS_CLK_DIV_REG.CLK_PLL1_DIV_EN);
        }else if(mux.div == CLK_DIV_NONE){
            result = clk_topgen_input_directly(mux.src ,on_off);
            goto src_ctrl;
        }
    }else if(mux.div == CLK_DIV_NONE){
        result = clk_topgen_input_directly(mux.src ,on_off);
        goto src_ctrl;
    }else{
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }

    DIV_EN = div_opt_reg_base+ shift ;
    p_res_mgr = div_idx == OSC_DIV_IDX ? ((__IO uint8_t *)&clock_domain.lposc_domain.field.osc):
                                         ((__IO uint8_t *)&clock_domain.dcxo_domain.field.upll.src_cnt);

    if(on_off){
        if(DIV_PWER && !*DIV_PWER){
            *DIV_PWER = 0x1;
        }

        if(DIV_PWER && !*DIV_EN){
            *DIV_EN = 0x1;
        }
        hal_gpt_delay_us(20);
        p_res_mgr[shift] += 1;

    }else if(p_res_mgr[shift]){
        p_res_mgr[shift] -= 1;

        if(! p_res_mgr[shift]){
            *DIV_EN = 0x0;
            __IO uint32_t *src_div_status = (__IO uint32_t*)p_res_mgr;

            if(DIV_PWER && !(*src_div_status)){
                *DIV_PWER = 0x0;
            }
        }
    }
src_ctrl:

    if(src_hdr.post_hdr && !sta && src_hdr.usr_sta()){
        src_hdr.post_hdr();
    }

    return result;
}

ATTR_TEXT_IN_TCM static void clock_top_mux_ctrl(clock_mux_sel_id mux_id,uint8_t target_sel){

    volatile uint8_t *sel = NULL;
    volatile uint8_t *chg = NULL;
    volatile uint8_t *chg_ok = NULL;
    volatile uint8_t *force_on = NULL;
    switch (mux_id) {
    case CLK_SYS_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_SYS_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_SYS_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_SYS);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_SYS_OK);
        break;
    case CLK_DSP_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_DSP_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_DSP_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_DSP);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_DSP_OK);
        break;
    case CLK_SFC_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_SFC_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_SFC_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_SFC);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_SFC_OK);
        break;
    case CLK_ESC_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_ESC_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_ESC_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_ESC);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_ESC_OK);
        break;
    case CLK_SPIMST0_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_SPIMST0_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_SPIMST0_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_SPIMST0);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_SPIMST0_OK);
        break;
    case CLK_SPIMST1_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_SPIMST1_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_SPIMST1_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_SPIMST1);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_SPIMST1_OK);
        break;
    case CLK_SPIMST2_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_SPIMST2_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_SPIMST2_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_SPIMST2);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_SPIMST2_OK);
        break;
    case CLK_SPISLV_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_SPISLV_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_SPISLV_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_SPISLV);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_SPISLV_OK);
        break;
    case CLK_SDIOMST0_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_SDIOMST0_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_SDIOMST0_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_SDIOMST0);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_SDIOMST0_OK);
        break;
    case CLK_USB_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_USB_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_USB_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_USB);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_USB_OK);
        break;
    case CLK_AUD_BUS_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_AUD_BUS_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_AUD_BUS_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_AUD_BUS);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_AUD_BUS_OK);
        break;
    case CLK_AUD_GPSRC_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_AUD_GPSRC_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_AUD_GPSRC_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_AUD_GPSRC);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_AUD_GPSRC_OK);
        break;
    case CLK_AUD_ULCK_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_AUD_ULCK_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_AUD_ULCK_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_AUD_ULCK);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_AUD_ULCK_OK);
        break;
    case CLK_AUD_DLCK_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_AUD_DLCK_SEL);
        force_on = BYTE_REG(CKSYS_CLK_FORCE_ON_REG.CLK_AUD_DLCK_FORCE_ON);
        chg =      BYTE_REG(CKSYS_CLK_UPDATE_REG.CHG_AUD_DLCK);
        chg_ok =   BYTE_REG(CKSYS_CLK_UPDATE_STATUS_REG.CLK_AUD_DLCK_OK);
        break;
    case CLK_VOW_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_VOW_SEL);
        break;
    case CLK_AUD_INTERFACE0_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_AUD_INTF0_SEL);
        break;
    case CLK_AUD_INTERFACE1_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_AUD_INTF1_SEL);
        break;
    case CLK_AUD_ENGINE_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_AUD_ENGINE_SEL);
        break;
    case CLK_26M_SEL:
        sel =      BYTE_REG(CKSYS_CLK_CFG_REG.CLK_26M_SEL);
        break;

    default :
        return ;
    }
    // n to 1
    if (sel) {
        *sel = target_sel;
    }
    if (force_on) {
        *force_on = 1;
    }
#ifndef FPGA_ENV
    if (chg) {
        *chg = 1;
        while (*chg);
    }
    if (chg_ok) {
        while (*chg_ok);
    }
#endif
    if (force_on) {
        *force_on = 0;
    }
}

/*************************************************************************
 * Clock mux select API definition part
 * 1. Enable individual clock divider
 * 2. Force clock on th prevent clock can't switch to target clock
 * 3. Set CSW to target clock freq. and set change bit
 * 4. After clock change to target freq. Change bit will be cleared to0 and release clock gating
 * 5. Disable forced on clock
 *************************************************************************/

ATTR_TEXT_IN_TCM hal_clock_status_t clock_mux_sel(clock_mux_sel_id mux_id, uint32_t mux_sel){
    uint8_t xo_sel = 0,target_sel =0;
    uint32_t irq_mask = 0;
    hal_clock_status_t result = HAL_CLOCK_STATUS_OK;
    pin_mux_t **sta = NULL;
    top_mux_info *top_clk_mux = NULL;
    clk_src_handler src_hdr = {NULL,NULL};
    clk_usr_status usr_sta = nonexist ;
    if(TOPGEN_CLK_DOMAIN(mux_id)){
        if(mux_id > CLK_AUD_INTERFACE1_SEL)
            return HAL_CLOCK_STATUS_INVALID_PARAMETER;

        top_clk_mux =&clock_domain.top_mux_info_list[CLK_SPIMST0_SEL];
        switch(mux_id){
        case CLK_SPIMST0_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_0;
            break;
        case CLK_SPIMST1_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_1;
            break;
        case CLK_SPIMST2_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_2;
            break;
        case CLK_SPISLV_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_s;
            break;
        case CLK_AUD_INTERFACE0_SEL:
            top_clk_mux =&clock_domain.top_mux_info_list[CLK_AUD_INTERFACE0_SEL];
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_0;
            break;
        case CLK_AUD_INTERFACE1_SEL:
            top_clk_mux =&clock_domain.top_mux_info_list[CLK_AUD_INTERFACE0_SEL];
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_1;
            break;
        default :
            if(mux_id >= NR_MUXS)
                return HAL_CLOCK_STATUS_INVALID_PARAMETER;

            if(mux_id == CLK_MCLK_SEL){
                xo_sel = 2;
            }
            top_clk_mux = &clock_domain.top_mux_info_list[mux_id];
            sta = (pin_mux_t**)&top_clk_mux->cur_sel_0;
        }
    }else if(PERIPHERAL_CLK_DOMAIN(mux_id)){
        top_clk_mux =&clock_domain.peri_mux_info_list[(CLK_PWM0_SEL & 0x1f)];
        switch(mux_id){
        case CLK_PWM0_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_0;
            break;
        case CLK_PWM1_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_1;
            break;
        case CLK_PWM2_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_2;
            break;
        case CLK_PWM3_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_3;
            break;
        case CLK_PWM4_SEL:
            sta = (pin_mux_t**)&top_clk_mux -> cur_sel_s;
            break;
        default :
            if(mux_id >= CLK_PERI_NUM)
                return HAL_CLOCK_STATUS_INVALID_PARAMETER;
            top_clk_mux = &clock_domain.peri_mux_info_list[(mux_id & 0x1f)];
            sta = (pin_mux_t**)&top_clk_mux->cur_sel_0;
            break;
        }
    }

    pin_mux_t *next_sta = NULL;
    if(TOPGEN_CLK_DOMAIN(mux_id) && hal_clock_is_enabled(clk_linkage_tbl[mux_id]) == false){
        next_sta = (top_clk_mux->sels) + mux_sel;
        goto cursor_shift;
    }

    if(mux_sel != CLK_ENABLE){
        // clock disable or normal mux switch
        target_sel = mux_sel == CLK_DISABLE ? xo_sel : mux_sel;
        next_sta = (top_clk_mux->sels) + target_sel;
    }

    if((*sta) != next_sta){

        if(next_sta){
            // clock disable or normal mux switch
            switch((*sta)->src){

            case CLK_APLL1:
                src_hdr = (clk_src_handler){clock_get_apll1_status,apll1_disable};
                usr_sta = src_hdr.usr_sta();
                break;
            case CLK_APLL2:
                src_hdr = (clk_src_handler){clock_get_apll2_status,apll2_disable};
                usr_sta = src_hdr.usr_sta();
                break;

            case CLK_UPLL:
                src_hdr = (clk_src_handler){clock_get_upll_status,upll_disable};
                usr_sta = src_hdr.usr_sta();
                break;
            case CLK_OSC:
                src_hdr = (clk_src_handler){clock_get_lposc_status,lposc_disable};
                usr_sta = src_hdr.usr_sta();
                break;
            default :
                break;
            }
        }else{
            //clock enable
            next_sta = *sta;
            target_sel = next_sta - (top_clk_mux->sels);
            hal_nvic_save_and_set_interrupt_mask_special(&irq_mask);
            *sta = NULL;
        }

        if((result = clock_div_ctrl(*next_sta,true)) != HAL_CLOCK_STATUS_OK){
            return result;
        }

        if(TOPGEN_CLK_DOMAIN(mux_id))
            clock_top_mux_ctrl(mux_id,target_sel);


        if(*sta)
            result = clock_div_ctrl(**sta,false);

        if(src_hdr.post_hdr && usr_sta && !(src_hdr.usr_sta())){
            src_hdr.post_hdr();
        }

cursor_shift:
        if(mux_sel != CLK_DISABLE){
            *sta = next_sta;
            if(mux_sel == CLK_ENABLE){
                hal_nvic_restore_interrupt_mask_special(irq_mask);
            }
        }
    }
    return result;
}


uint8_t clock_mux_cur_sel(clock_mux_sel_id mux_id){
    switch (mux_id) {
    case CLK_SYS_SEL:            return CKSYS_CLK_CFG_REG.CLK_SYS_SEL;
    case CLK_DSP_SEL:            return CKSYS_CLK_CFG_REG.CLK_DSP_SEL;
    case CLK_SFC_SEL:            return CKSYS_CLK_CFG_REG.CLK_SFC_SEL;
    case CLK_ESC_SEL:            return CKSYS_CLK_CFG_REG.CLK_ESC_SEL;
    case CLK_SPIMST0_SEL:        return CKSYS_CLK_CFG_REG.CLK_SPIMST0_SEL;
    case CLK_SPIMST1_SEL:        return CKSYS_CLK_CFG_REG.CLK_SPIMST1_SEL;
    case CLK_SPIMST2_SEL:        return CKSYS_CLK_CFG_REG.CLK_SPIMST2_SEL;
    case CLK_SPISLV_SEL:         return CKSYS_CLK_CFG_REG.CLK_SPISLV_SEL;
    case CLK_SDIOMST0_SEL:       return CKSYS_CLK_CFG_REG.CLK_SDIOMST0_SEL;
    case CLK_USB_SEL:            return CKSYS_CLK_CFG_REG.CLK_USB_SEL;
    case CLK_AUD_BUS_SEL:        return CKSYS_CLK_CFG_REG.CLK_AUD_BUS_SEL;
    case CLK_AUD_GPSRC_SEL:      return CKSYS_CLK_CFG_REG.CLK_AUD_GPSRC_SEL;
    case CLK_AUD_ULCK_SEL:       return CKSYS_CLK_CFG_REG.CLK_AUD_ULCK_SEL;
    case CLK_AUD_DLCK_SEL:       return CKSYS_CLK_CFG_REG.CLK_AUD_DLCK_SEL;
    case CLK_VOW_SEL:            return CKSYS_CLK_CFG_REG.CLK_VOW_SEL;
    case CLK_AUD_INTERFACE0_SEL: return CKSYS_CLK_CFG_REG.CLK_AUD_INTF0_SEL;
    case CLK_AUD_INTERFACE1_SEL: return CKSYS_CLK_CFG_REG.CLK_AUD_INTF1_SEL;
    case CLK_AUD_ENGINE_SEL:     return CKSYS_CLK_CFG_REG.CLK_AUD_ENGINE_SEL;
    case CLK_26M_SEL:            return CKSYS_CLK_CFG_REG.CLK_26M_SEL;
    default :                    return 0;
    }
}

#endif


ATTR_TEXT_IN_TCM uint32_t clock_freq_meter(ref_clock REF_CLK,hal_src_clock SRC_CLK,uint32_t winset,uint32_t REF_CLK_frequency){
    PLL_ABIST_FQMTR_COM_REG.FQMTR_EN = 0x0;
    PLL_ABIST_FQMTR_COM_REG.FQMTR_RST = 0x1;
    PLL_ABIST_FQMTR_COM_REG.FQMTR_RST = 0x0;
    *CKSYS_TST_SEL_1 = (REF_CLK <<8) + SRC_CLK;
    PLL_ABIST_FQMTR_COM_REG.FQMTR_WINSET = winset;
    PLL_ABIST_FQMTR_COM_REG.FQMTR_EN = 0x1;
    hal_gpt_delay_us(5);
#ifndef FPGA_ENV
    while(PLL_ABIST_FQMTR_COM_REG.PLL_ABIST_FQMTR_CON1_ & 0x80);
#endif
    return REF_CLK_frequency * (*PLL_ABIST_FQMTR_DATA) / (winset + 1);
}

ATTR_TEXT_IN_TCM uint32_t hal_clock_get_freq_meter(hal_src_clock SRC_CLK,uint32_t winset){
    return clock_freq_meter(XO_CK,SRC_CLK,winset,26000);
}
#ifndef FPGA_ENV
ATTR_TEXT_IN_TCM static uint8_t osc_search_cali(osc_cali_t cali_level) /* worst case = 6 times*/
{
    uint8_t low ,high,mid,cali,scale;
    uint32_t result,diff;
    uint32_t target;
    cali_info_t *cali_ctx = osc_tune_tbl;
process :
    target = cali_ctx->cali_target;
    scale = cali_ctx -> scale;
    mid = 0; cali = 0;result =hal_clock_get_freq_meter(cali_ctx->src,10110);
    if(result >= target){
        high = scale == lposc_cali_scale ?  LPOSC_CON_REG.RG_HFOSC_CALI : LPOSC_CON_REG.RG_HFOSC_FT;
        diff = result -target ;
        low = 0;
    }else{
        high = scale;
        diff = target - result ;
        low = scale == lposc_cali_scale ?  LPOSC_CON_REG.RG_HFOSC_CALI : LPOSC_CON_REG.RG_HFOSC_FT;
    }

    while ((low <= high) && (high <= scale))
    {
        mid = ((high-low)>> cali_level) + low; /* replace (high+low)/2, to avoid overflow */
        if(cali_ctx->scale == lposc_cali_scale){
            LPOSC_CON_REG.RG_HFOSC_CALI = mid;
        }else if(cali_ctx->scale == lposc_ft_scale){
            LPOSC_CON_REG.RG_HFOSC_FT = mid;
        }
        hal_gpt_delay_us(5);
        result = hal_clock_get_freq_meter(cali_ctx->src,10110);

        if (result == target){
            return mid;
        }
        else if (result > target){
            high = mid - 1;
            if (result-target < diff){
                cali = mid;
                diff = result-target;
            }
        }
        else if (result < target){
            low = mid + 1;
            if (target-result < diff){
                cali = mid;
                diff = target-result;
            }
        }
    }
    if(cali_ctx->scale == lposc_cali_scale){
        cali_ctx ++;
        goto process;
    }else if(cali_ctx->scale == lposc_ft_scale){
        if(!LPOSC_CON_REG.RG_HFOSC_FT){
            LPOSC_CON_REG.RG_HFOSC_CALI --;
            LPOSC_CON_REG.RG_HFOSC_FT = 0xf;
            goto process;
        }else{
            result= hal_clock_get_freq_meter(13,10110);
            if(result < osc_tune_tbl[1].cali_target && osc_tune_tbl[1].cali_target - result > FT_SCALE){
                if(LPOSC_CON_REG.RG_HFOSC_FT == 0x1f){
                    LPOSC_CON_REG.RG_HFOSC_CALI ++;
                    LPOSC_CON_REG.RG_HFOSC_FT = 0x0;
                    log_hal_msgid_info("[lposc cali] carry_in\r\n",0);
                }else{
                    LPOSC_CON_REG.RG_HFOSC_FT ++;
                }
            }else if(result > osc_tune_tbl[1].cali_target && result - osc_tune_tbl[1].cali_target > FT_SCALE){
                LPOSC_CON_REG.RG_HFOSC_FT --;
            }
        }
    }
    return cali;
}
#if 0
typedef union{
    uint16_t value[3];
    struct{
        uint8_t cali_min;
        uint8_t ft_min;
        uint8_t cali_mid;
        uint8_t ft_mid;
        uint8_t cali_max;
        uint8_t ft_max;
    }field;
}hopping_range_t;

static void search_hopping_range(float drift,hopping_range_t *hopping){
    uint32_t temp_target[2] = {0};
    temp_target[0] = osc_tune_tbl[0].cali_target;
    temp_target[1] = osc_tune_tbl[1].cali_target;
    hopping->field.cali_max = LPOSC_CON_REG.RG_HFOSC_CALI;
    hopping->field.ft_max = LPOSC_CON_REG.RG_HFOSC_FT;

    osc_tune_tbl[0].cali_target *= (1 - drift);
    osc_tune_tbl[1].cali_target *= (1 - drift);
    osc_search_cali(COARSE_CALI);
    osc_search_cali(COARSE_FT);
    hopping->field.cali_mid = LPOSC_CON_REG.RG_HFOSC_CALI;
    hopping->field.ft_mid = LPOSC_CON_REG.RG_HFOSC_FT;

    osc_tune_tbl[0].cali_target = temp_target[0] *(1 - (drift*2));
    osc_tune_tbl[1].cali_target = temp_target[1] *(1 - (drift*2));
    osc_search_cali(COARSE_CALI);
    osc_search_cali(COARSE_FT);
    hopping->field.cali_min = LPOSC_CON_REG.RG_HFOSC_CALI;
    hopping->field.ft_min = LPOSC_CON_REG.RG_HFOSC_FT;

    LPOSC_CON_REG.RG_HFOSC_CALI = hopping->field.cali_max;
    LPOSC_CON_REG.RG_HFOSC_FT = hopping->field.ft_max;

}

void lposc_hopping(float drift){
    if(drift >= 1)
        return;

    hopping_range_t hopping = {.value= {0}};
    search_hopping_range(drift,&hopping);
    uint32_t lposc_fhset = (hopping.field.cali_max <<5 )| hopping.field.ft_max;
    SSC_CON.HFOSC_FHSET = lposc_fhset;
    SSC_CON.HFOSC_SFSTR_EN = 0x1;
    SSC_CON.HFOSC_SFSTR_DTS= 0x0;
    SSC_CON.HFOSC_SFSTR_DYS= 0x3;
    SSC_CON.HFOSC_FREQ_STR = ~SSC_CON.HFOSC_FREQ_STR;
    hal_gpt_delay_us(150);

    SSC_CON.HFOSC_FHCTL_EN = 0x1;
    lposc_fhset = (hopping.field.cali_mid << 5 )| hopping.field.ft_mid;
    SSC_CON.HFOSC_FHSET = lposc_fhset;
    SSC_CON.HFOSC_SFSTR_EN = 0x1;
    SSC_CON.HFOSC_SFSTR_DTS = 0x0;
    SSC_CON.HFOSC_SFSTR_DYS = 0x2;
    SSC_CON.HFOSC_FREQ_STR = ~SSC_CON.HFOSC_FREQ_STR;
    hal_gpt_delay_us(50);

    SSC_CON.HFOSC_FRSSC_DTS=0x3;
    SSC_CON.HFOSC_FRSSC_DYS =0x0;
    /*SSC_CON.HFOSC_FRSSC_UPLMT = ((hopping.field.cali_max-hopping.field.cali_mid)) <<5
                                 |(hopping.field.ft_max-hopping.field.ft_mid);
    SSC_CON.HFOSC_FRSSC_DNLMT = ((hopping.field.cali_mid-hopping.field.cali_min)) <<5
                                 |(hopping.field.ft_mid-hopping.field.ft_min);*/
    SSC_CON.HFOSC_FRSSC_UPLMT =  ((hopping.field.cali_max <<5) | hopping.field.ft_max)-((hopping.field.cali_mid <<5)|hopping.field.ft_mid);
    SSC_CON.HFOSC_FRSSC_DNLMT = ((hopping.field.cali_mid <<5) | hopping.field.ft_mid)-((hopping.field.cali_min <<5)|hopping.field.ft_min);         
    SSC_CON.HFOSC_FRSSC_EN = 0x1;
}
#endif 
#endif

#ifdef __EXT_DA__
#define DVFS_INIT_LEVEL   2
#else 
#define DVFS_INIT_LEVEL   3
#endif
ATTR_TEXT_IN_TCM void lposc_init(void){
#ifndef FPGA_ENV
    osc_search_cali(COARSE_CALI);
    osc_search_cali(COARSE_FT);
#endif
    CLKSQ_CON_REG.BP_PLL_DLY = 0x0;
    CLKSQ_CON_REG.LPOSC_EN_SEL = 0x0;
}

ATTR_TEXT_IN_TCM static void lposc_enable(){
    LPOSC_CON_REG.RG_HFOSC_CALI = 0x1F;
    LPOSC_CON_REG.RG_HFOSC_FT = 0xF;
    LPOSC_CON_REG.RG_HFOSC_BIAS = 0x41;
    LPOSC_CON_REG.DA_HFOSC_EN = 0x1;
    LPOSC_CON_REG.RG_HFOSC_160M_EN = 0x1; //for calibration
    hal_gpt_delay_us(20);
    CLKSQ_CON_REG.RG_LPOSC_RDY = 0x1;
    lposc_init();
}

#ifdef MTK_DVFS_LOWV_OP73V
#define DVFS_LOWV_CLK_DESCRIPTION        {.field = { .sys = 4,.dsp = 5,.sfc = 2,.esc = 0}}
#define DVFS_LOWV_TEST_CLK_DESCRIPTION   {.field = { .sys = 4,.dsp = 4,.sfc = 2,.esc = 0}}
#else
#define DVFS_LOWV_CLK_DESCRIPTION        {.field = { .sys = 4,.dsp = 4,.sfc = 2,.esc = 0}}
#define DVFS_LOWV_TEST_CLK_DESCRIPTION   {.field = { .sys = 4,.dsp = 5,.sfc = 2,.esc = 0}}
#endif

ATTR_RWDATA_IN_TCM clk_volt_lv_info dvfs_freq_tbl[VOLT_LV_NUM] = {DVFS_LOWV_TEST_CLK_DESCRIPTION,
                                                                  DVFS_LOWV_CLK_DESCRIPTION,
#ifdef HAL_ESC_MODULE_ENABLED
                                                                  {.field = { .sys = 3,.dsp = 3,.sfc = 2,.esc = 2}},
                                                                  {.field = { .sys = 2,.dsp = 2,.sfc = 2,.esc = 2}}};
#else
                                                                  {.field = { .sys = 3,.dsp = 3,.sfc = 2,.esc = 0}},
                                                                  {.field = { .sys = 2,.dsp = 2,.sfc = 2,.esc = 0}}};
#endif

#if !defined(__EXT_BOOTLOADER__) || defined(__EXT_DA__)


#define control_block(div)   div > DIV_HIGH_TYPE ? (clock_domain.lposc_domain.field.free) : ((__IO uint8_t *)&clock_domain.lposc_domain.field.osc)[div]
#define CUR_SEL(mux)         sys_div_tbl[div_cur.field.mux]
#define NEXT_SEL(mux)        sys_div_tbl[div_next.field.mux]
#define status_modify(mux)   if(div_chg.field.mux){\
                                 control_block(NEXT_SEL(mux))++;\
                                 control_block(CUR_SEL(mux))--;\
                             }

typedef enum{
    DIV_LOW_TYPE = 0,
    DIV_MIDDLE_TYPE,
    DIV_HIGH_TYPE,
    NONE_DIV_TYPE,
}div_type_e;

ATTR_RODATA_IN_TCM uint8_t sys_div_tbl[6] = {NONE_DIV_TYPE,
                                             DIV_LOW_TYPE,
                                             NONE_DIV_TYPE,
                                             DIV_LOW_TYPE,
                                             DIV_LOW_TYPE,
                                             DIV_HIGH_TYPE};


ATTR_TEXT_IN_TCM static hal_clock_status_t clock_freq_sw_switch(clk_volt_lv_info div_cur,clk_volt_lv_info div_next){
    hal_clock_status_t result = HAL_CLOCK_STATUS_OK;
    clk_volt_lv_info div_chg = {.value = 0};
    div_chg.value = (div_next.value ^ div_cur.value);
    status_modify(dsp);
    status_modify(sys);
    //status_modify(sfc);
#ifdef HAL_ESC_MODULE_ENABLED
    status_modify(esc);
#endif

    if(!clock_domain.lposc_domain.field.osc.field.div_low)
        CKSYS_CLK_DIV_REG.CLK_OSC1_D2_EN = 0;
    if(!clock_domain.lposc_domain.field.osc.field.div_middle)
        CKSYS_CLK_DIV_REG.CLK_OSC1_D3_EN = 0;
    if(!clock_domain.lposc_domain.field.osc.field.div_high)
        CKSYS_CLK_DIV_REG.CLK_OSC1_D5_EN = 0;
    return result;
}
#endif


ATTR_TEXT_IN_TCM void dvfs_switch_clock_freq(hal_clock_volt_lv cur_lv,hal_clock_volt_lv next_lv){
#if !defined(__EXT_BOOTLOADER__) /*|| defined(__EXT_DA__)*/
    clk_volt_lv_info div_next = {.value = dvfs_freq_tbl[next_lv].value};
    clk_volt_lv_info div_cur = {.value = dvfs_freq_tbl[cur_lv].value};

    *CKSYS_CLK_CFG_0 = div_next.value;
#else
    *CKSYS_CLK_CFG_0 = dvfs_freq_tbl[next_lv].value;
#endif
#ifdef HAL_ESC_MODULE_ENABLED
#define DVFS_FORCE_ON_MASK 0x1010101
    *CKSYS_CLK_FORCE_ON_0 = DVFS_FORCE_ON_MASK;
    *CKSYS_CLK_UPDATE_0 |= 0x1000000;
    *CKSYS_CLK_UPDATE_0 |= 0x10000;
    *CKSYS_CLK_UPDATE_0 |= 0x100;
    *CKSYS_CLK_UPDATE_0 |= 0x1;
#else
#define DVFS_FORCE_ON_MASK 0x10101
    *CKSYS_CLK_FORCE_ON_0 = DVFS_FORCE_ON_MASK;
    *CKSYS_CLK_UPDATE_0 |= 0x10000;
    *CKSYS_CLK_UPDATE_0 |= 0x100;
    *CKSYS_CLK_UPDATE_0 |= 0x1;
#endif
    while(*CKSYS_CLK_UPDATE_0 & DVFS_FORCE_ON_MASK);
    while(*CKSYS_CLK_UPDATE_STATUS_0 & DVFS_FORCE_ON_MASK);
    *CKSYS_CLK_FORCE_ON_0 = 0;
#if !defined(__EXT_BOOTLOADER__) /*|| defined(__EXT_DA__)*/
    clock_freq_sw_switch(div_cur,div_next);
#ifdef HAL_ESC_MODULE_ENABLED
    esc_set_freq_related_parameter(div_next.field.esc);
#endif
#endif
}




hal_clock_status_t clk_mux_init(){

    hal_clock_status_t result = HAL_CLOCK_STATUS_OK;
#if !defined(__EXT_BOOTLOADER__) /*|| defined(__EXT_DA__)*/
    uint8_t xo_sel = 0;
    clock_mux_sel_id mux_id = CLK_SYS_SEL;
    for(;mux_id < NR_MUXS; mux_id++){
        xo_sel = (mux_id == CLK_MCLK_SEL) ? 2:0;
        pin_mux_t *init_sel = (pin_mux_t*)&clock_domain.top_mux_info_list[mux_id].sels[xo_sel];
        clock_domain.top_mux_info_list[mux_id].cur_sel_0 = init_sel;
        clock_domain.top_mux_info_list[mux_id].cur_sel_1 = init_sel;
        clock_domain.top_mux_info_list[mux_id].cur_sel_2 = init_sel;
        clock_domain.top_mux_info_list[mux_id].cur_sel_3 = init_sel;
        clock_domain.top_mux_info_list[mux_id].cur_sel_s = init_sel;
    }
    
    xo_sel = 0;
    for(mux_id = CLK_PWM0_SEL;mux_id < CLK_PERI_NUM; mux_id++){
        pin_mux_t *init_sel = (pin_mux_t*)&clock_domain.peri_mux_info_list[(mux_id & 0x1f)].sels[xo_sel];
        clock_domain.peri_mux_info_list[mux_id & 0x1f].cur_sel_0 = init_sel;
        clock_domain.peri_mux_info_list[mux_id & 0x1f].cur_sel_1 = init_sel;
        clock_domain.peri_mux_info_list[mux_id & 0x1f].cur_sel_2 = init_sel;
        clock_domain.peri_mux_info_list[mux_id & 0x1f].cur_sel_3 = init_sel;
        clock_domain.peri_mux_info_list[mux_id & 0x1f].cur_sel_s = init_sel;
    }

    clock_mux_sel(CLK_SYS_SEL,clock_mux_cur_sel(CLK_SYS_SEL));
    clock_mux_sel(CLK_DSP_SEL,clock_mux_cur_sel(CLK_DSP_SEL));
    clock_mux_sel(CLK_SFC_SEL,clock_mux_cur_sel(CLK_SFC_SEL));
#ifdef HAL_ESC_MODULE_ENABLED
    clock_mux_sel(CLK_ESC_SEL,clock_mux_cur_sel(CLK_ESC_SEL));
#endif
#else
    lposc_enable();
    CKSYS_CLK_DIV_REG.CLK_OSC1_DIV_EN = 0x1;
    CKSYS_CLK_DIV_REG.CLK_OSC1_D2_EN = 0x1;
    hal_gpt_delay_us(20);
    dvfs_switch_clock_freq(0,DVFS_INIT_LEVEL);
#endif
    return result;
}


void hal_clock_dcm_enable(hal_clk_dcm_id dcm_id){
    switch(dcm_id){
    case clk_aud_bus_dcm:
        BUS_DCM_CON_REG.RG_BUS_DCM_EN |= 0x8;
        break;
    case clk_dsp_dcm :
        *DSP0_SLOW_CON0 = 0x1;
        BUS_DCM_CON_REG.RG_BUS_DCM_EN |= 0x4;
        break;
    case clk_bus_dcm : //BUS DCM
        //slow bus dcm
        XO_DCM_CON_REG.RG_XO_DCM_DBC_NUM = 0x2;
        XO_DCM_CON_REG.RG_XO_DCM_DBC_EN = 0x1;
        XO_DCM_CON_REG.RG_XO_CLKSLOW_EN = 0x1;
        XO_DCM_CON_REG.RG_XO_CLKOFF_EN = 0x1;
        BUS_DCM_CON_REG.RG_BUS_DCM_EN |= 0x2;

        // fast bus dcm
        BUS_DCM_CON_REG.RG_BUS_DCM_DBC_NUM =0x2;
        BUS_DCM_CON_REG.RG_BUS_DCM_DBC_EN= 0x1;
        BUS_DCM_CON_REG.RG_BUS_CLKSLOW_EN = 0x1;
        BUS_DCM_CON_REG.RG_BUS_CLKOFF_EN=0x1;

        *CLK_FREQ_SWCH |= 0x1;
        hal_gpt_delay_us(1);
        break;
    case clk_sfc_dcm :
        SFC_DCM_CON_REG.RG_SFC_DCM_DBC_NUM = 0xFF;
        SFC_DCM_CON_REG.RG_SFC_DCM_DBC_EN = 0x1;
        SFC_DCM_CON_REG.RG_SFC_CLK_OFF_EN = 0x1;
        SFC_DCM_CON_REG.RG_SFC_DCM_APB_SEL = 0x6;
        SFC_DCM_CON_REG.RG_SFC_DCM_APB_TOG = ~SFC_DCM_CON_REG.RG_SFC_DCM_APB_TOG;
        break;

    case clk_esc_dcm :
        ESC_DCM_CON_REG.RG_ESC_DCM_DBC_NUM = 0xFF;
        ESC_DCM_CON_REG.RG_ESC_DCM_DBC_EN = 0x1;
        ESC_DCM_CON_REG.RG_ESC_CLK_OFF_EN = 0x1;
        ESC_DCM_CON_REG.RG_ESC_DCM_APB_SEL = 0x6;
        ESC_DCM_CON_REG.RG_ESC_DCM_APB_TOG = ~ESC_DCM_CON_REG.RG_ESC_DCM_APB_TOG;
        break;
    case clk_cm4_dcm :
        CM4_DCM_CON_REG.RG_CM_CLKSLOW_EN = 0x1;
        hal_gpt_delay_us(1);
        break;
    default :
        return;
    }

    hal_gpt_delay_us(1);
}


void hal_clock_set_pll_dcm_init(void){
    hal_clock_dcm_enable(clk_sfc_dcm);
    //hal_clock_dcm_enable(clk_aud_bus_dcm);
    hal_clock_dcm_enable(clk_dsp_dcm);
    hal_clock_dcm_enable(clk_bus_dcm);
    hal_clock_dcm_enable(clk_esc_dcm);
    hal_clock_dcm_enable(clk_cm4_dcm);
}
#if !defined(__EXT_BOOTLOADER__) || defined(__EXT_DA__)
ATTR_TEXT_IN_TCM static void apll2_cali(){
    *XPLL_DBG_PROB = 0x8;
    APLL2_CON_REG.RG_APLL1_VTMON_EN = 0x1;
#ifndef FPGA_ENV
    while(!(*XPLL_DBG_PROB_MON & 0x8)){
        if (*XPLL_DBG_PROB_MON & 0x2){
            APLL2_CON_REG.RG_APLL1_IBANK_FINETUNE += 1;
            if(APLL2_CON_REG.RG_APLL1_IBANK_FINETUNE == 0xF){
                break;
            }
            hal_gpt_delay_us(50);
        }else if(*XPLL_DBG_PROB_MON & 0x4){
            APLL2_CON_REG.RG_APLL1_IBANK_FINETUNE -= 1;
            if(!APLL2_CON_REG.RG_APLL1_IBANK_FINETUNE){
                break;
            }
            hal_gpt_delay_us(50);
        }else{
            log_hal_msgid_info("apll cali fail \r\n",0);
            break;
        }
    }
#endif
    *XPLL_DBG_PROB = 0x0;
    APLL2_CON_REG.RG_APLL1_VTMON_EN = 0x0;
}

ATTR_TEXT_IN_TCM static void apll1_cali(){
    *XPLL_DBG_PROB = 0x1;
    APLL1_CON_REG.RG_APLL1_VTMON_EN = 0x1;
#ifndef FPGA_ENV
    while(!(*XPLL_DBG_PROB_MON & 0x8)){
        if (*XPLL_DBG_PROB_MON & 0x2){
            APLL1_CON_REG.RG_APLL1_IBANK_FINETUNE += 1;
            if(APLL1_CON_REG.RG_APLL1_IBANK_FINETUNE == 0xF){
                break;
            }
            hal_gpt_delay_us(50);
        }else if(*XPLL_DBG_PROB_MON & 0x4){
            APLL1_CON_REG.RG_APLL1_IBANK_FINETUNE -= 1;
            if(!APLL1_CON_REG.RG_APLL1_IBANK_FINETUNE){
                break;
            }
            hal_gpt_delay_us(50);
        }else{
            log_hal_msgid_info("apll cali fail \r\n",0);
            break;
        }
    }
#endif
    *XPLL_DBG_PROB = 0x0;
    APLL1_CON_REG.RG_APLL1_VTMON_EN = 0x0;
}

ATTR_TEXT_IN_TCM static void upll_cali(){
    *ABIST_LMON_SEL = 0x13;
    UPLL_CON_REG.RG_UPLL_VTMON_EN = 0x1;
#ifndef FPGA_ENV
    while(!(*ABIST_MON_DATA0 & 0x4)){
        if (*ABIST_MON_DATA0 & 0x2){
            UPLL_CON_REG.RG_UPLL_IBANK_FINETUNE += 1;
            if(UPLL_CON_REG.RG_UPLL_IBANK_FINETUNE == 0xF){
                break;
            }
            hal_gpt_delay_us(50);
        }else if(*ABIST_MON_DATA0 & 0x1){
            UPLL_CON_REG.RG_UPLL_IBANK_FINETUNE -= 1;
            if(!UPLL_CON_REG.RG_UPLL_IBANK_FINETUNE){
                break;
            }
            hal_gpt_delay_us(50);
        }else{
            log_hal_msgid_info("upll cali fail \r\n",0);
            break;
        }
    }
#endif
    *ABIST_LMON_SEL = 0x0;
    UPLL_CON_REG.RG_UPLL_VTMON_EN = 0x0;
}

ATTR_TEXT_IN_TCM static uint8_t clk_pll_on(clock_pll_id pll_id){
    if( pll_id > CLK_OSC ){
        return 0;
    }
    hal_clock_enable(HAL_CLOCK_CG_MIXEDSYS);
    switch(pll_id){

        case CLK_APLL1:
            APLL1_CON_REG.RG_APLL1_V2I_EN = 0x1;
            APLL1_CON_REG.RG_APLL1_DDS_PWR_ON = 0x1;
            hal_gpt_delay_us(5);
            APLL1_CON_REG.RG_APLL1_DDS_ISO_EN = 0x0;
            APLL1_CON_REG.RG_APLL1_LCDDS_PCW_NCPO        = (apll_freq[CLK_APLL1] == CLK_APLL_45M)  ? 0x0DE517AA+1 : 0x0F1FAA4D+1;//? 45M+1 : 49M+1  /*? 0xCEC4EC5 : 0xE276276;    //APLL1_CON_REG[10] 42M : 46M*/
            APLL1_CON_REG.RG_APLL1_LCDDS_TUNER_PCW_NCPO  = (apll_freq[CLK_APLL1] == CLK_APLL_45M)  ? 0x0DE517AA-1 : 0x0F1FAA4D-1;//? 45M-1 : 49M-1  /*? 0xEC4EC4F : 0x104EC4EC;   //APLL1_CON_REG[14] 48M : 53M*/
            APLL1_CON_REG.RG_APLL1_DIV16_EN = 0x1;
            APLL1_CON_REG.RG_APLL1_IBANK_EN = 0X1;
            APLL1_CON_REG.RG_APLL1_CP_EN =0x1;
            APLL1_CON_REG.RG_APLL1_LCDDS_EN = 0x1;
            APLL1_CON_REG.RG_APLL1_EN= 0x1;
            APLL1_CON_REG.RG_APLL1_KVCO_SEL = 0x7;
            hal_gpt_delay_us(50);
            apll1_cali();

            APLL1_CON_REG.RG_APLL1_LCDDS_TUNER_EN = 0x1;
            break;
        case CLK_APLL2:
            APLL2_CON_REG.RG_APLL1_V2I_EN = 0x1;
            APLL2_CON_REG.RG_APLL1_DDS_PWR_ON = 0x1;
            hal_gpt_delay_us(5);
            APLL2_CON_REG.RG_APLL1_DDS_ISO_EN = 0x0;
            APLL2_CON_REG.RG_APLL1_LCDDS_PCW_NCPO        = (apll_freq[CLK_APLL2] == CLK_APLL_45M)  ? 0x0DE517AA+1 : 0x0F1FAA4D+1;//? 45M+1 : 49M+1  /*? 0xCEC4EC5 : 0xE276276;    //APLL1_CON_REG[10] 42M : 46M*/
            APLL2_CON_REG.RG_APLL1_LCDDS_TUNER_PCW_NCPO  = (apll_freq[CLK_APLL2] == CLK_APLL_45M)  ? 0x0DE517AA-1 : 0x0F1FAA4D-1;//? 45M-1 : 49M-1  /*? 0xEC4EC4F : 0x104EC4EC;   //APLL1_CON_REG[14] 48M : 53M*/
            APLL2_CON_REG.RG_APLL1_DIV16_EN = 0x1;
            APLL2_CON_REG.RG_APLL1_IBANK_EN = 0X1;
            APLL2_CON_REG.RG_APLL1_CP_EN =0x1;
            APLL2_CON_REG.RG_APLL1_LCDDS_EN = 0x1;
            APLL2_CON_REG.RG_APLL1_EN= 0x1;
            APLL2_CON_REG.RG_APLL1_KVCO_SEL = 0x7;
            hal_gpt_delay_us(50);
            apll2_cali();
            APLL2_CON_REG.RG_APLL1_LCDDS_TUNER_EN = 0x1;
            break;
        case CLK_UPLL:
            CLKSQ_CON_REG.BP_PLL_DLY = 0x0;
            UPLL_CON_REG.RG_UPLL_GLOBAL_LDO_BIAS_EN= 0x1;
            hal_gpt_delay_us(5);
            UPLL_CON_REG.RG_UPLL_IBANK_EN = 0x1;
            UPLL_CON_REG.RG_UPLL_CP_EN = 0x1;
            UPLL_CON_REG.RG_UPLL_PREDIV = 0x0;
            UPLL_CON_REG.RG_UPLL_FBDIV = 24;
            UPLL_CON_REG.RG_UPLL_POSTDIV = 0x1;
            UPLL_CON_REG.RG_UPLL_KVCO_SEL = 0x7; //meet BootRom flow 
            UPLL_CON_REG.DA_UPLL_EN = 0x1;
            UPLL_CON_REG.RG_UPLL_48M_EN = 0x1;
            hal_gpt_delay_us(20);
            CLKSQ_CON_REG.RG_UPLL_RDY = 0x1;
            CLKSQ_CON_REG.BP_PLL_DLY = 0x1;
            upll_cali();
            break;
        default :
            break;
    }
    hal_clock_disable(HAL_CLOCK_CG_MIXEDSYS);
    return 1;
}


ATTR_TEXT_IN_TCM static uint8_t clk_pll_off(clock_pll_id pll_id)
{
    if( pll_id >= NR_PLLS){
        return 0;
    }
    hal_clock_enable(HAL_CLOCK_CG_MIXEDSYS);
    switch(pll_id){
        case CLK_APLL1:
            APLL1_CON_REG.RG_APLL1_EN = 0x0;
            APLL1_CON_REG.RG_APLL1_LCDDS_EN = 0x0;
            APLL1_CON_REG.RG_APLL1_DDS_ISO_EN = 0x1;
            APLL1_CON_REG.RG_APLL1_DDS_PWR_ON = 0x0;
            hal_gpt_delay_us(5);

            APLL1_CON_REG.RG_APLL1_V2I_EN = 0x0;
            APLL1_CON_REG.RG_APLL1_LCDDS_PCW_NCPO_CHG = 0x0;
            // cksys tuner off
            APLL1_CON_REG.RG_APLL1_LCDDS_PCW_NCPO  = APLL1_CON_REG.DA_APLL1_LCDDS_PCW_NCPO;
            APLL1_CON_REG.RG_APLL1_LCDDS_TUNER_EN = 0x0;
            break;
        case CLK_APLL2:
            APLL2_CON_REG.RG_APLL1_EN = 0x0;
            APLL2_CON_REG.RG_APLL1_LCDDS_EN = 0x0;
            APLL2_CON_REG.RG_APLL1_DDS_ISO_EN = 0x1;
            APLL2_CON_REG.RG_APLL1_DDS_PWR_ON = 0x0;
            hal_gpt_delay_us(5);

            APLL2_CON_REG.RG_APLL1_V2I_EN = 0x0;
            APLL2_CON_REG.RG_APLL1_LCDDS_PCW_NCPO_CHG = 0x0;
            // cksys tuner off
            APLL2_CON_REG.RG_APLL1_LCDDS_PCW_NCPO  = APLL2_CON_REG.DA_APLL1_LCDDS_PCW_NCPO;
            APLL2_CON_REG.RG_APLL1_LCDDS_TUNER_EN = 0x0;
            break;
        case CLK_UPLL:
            CLKSQ_CON_REG.RG_UPLL_RDY = 0x0;
            UPLL_CON_REG.RG_UPLL_48M_EN = 0x0;
            UPLL_CON_REG.DA_UPLL_EN = 0x0;
            UPLL_CON_REG.RG_UPLL_IBANK_EN = 0x0;
            UPLL_CON_REG.RG_UPLL_CP_EN = 0x0;
            UPLL_CON_REG.RG_UPLL_GLOBAL_LDO_BIAS_EN = 0x0;
            break;
    }
    hal_clock_disable(HAL_CLOCK_CG_MIXEDSYS);
    return 1;
}

ATTR_TEXT_IN_TCM hal_clock_status_t clk_set_apll(clock_pll_id pll,clk_apll_freq apll){

    switch(apll){
    case CLK_APLL_45M:
        apll_freq[pll] = CLK_APLL_45M;
        goto pll_rst ;

    case CLK_APLL_49M:
        apll_freq[pll] = CLK_APLL_49M;
        goto pll_rst ;

    default :
        return HAL_CLOCK_STATUS_ERROR;
    }
pll_rst :

    if(pll ==CLK_APLL1 && APLL1_DOMAIN_USR_EXIST){
        clk_pll_off(CLK_APLL1);
        clk_pll_on(CLK_APLL1);
    }else if(pll ==CLK_APLL2 && APLL2_DOMAIN_USR_EXIST){
        clk_pll_off(CLK_APLL2);
        clk_pll_on(CLK_APLL2);
    }
    return HAL_CLOCK_STATUS_OK;
}

void chop_clock_sel(uint32_t div){
    uint8_t tog = CKSYS_CLK_DIV_REG.CHOP_DIV_CHG;
    CKSYS_CLK_DIV_REG.CHOP_DIV_SEL = div;
    CKSYS_CLK_DIV_REG.CHOP_DIV_CHG = ~tog;
    CKSYS_CLK_DIV_REG.CHOP_DIV_EN = 0x1;
    hal_gpt_delay_us(div);
}
#endif 
/*************************************************************************
 * CG Clock API definition
 *************************************************************************/

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_enable(hal_clock_cg_id clock_id)
{
    hal_clock_status_t ret = HAL_CLOCK_STATUS_OK;
    if(hal_clock_is_enabled(clock_id))
        return ret;
    volatile uint32_t *clr_addr = NULL;
    uint32_t bit_idx = clock_id & (0x1f);

    if (clock_id <= HAL_CLOCK_CG_AUXADC) {
        clr_addr = XO_PDN_PD_CLRD0;
    } else if (clock_id <= HAL_CLOCK_CG_GPTIMER) {
        clr_addr = XO_PDN_AO_CLRD0;
    } else if (clock_id <= HAL_CLOCK_CG_DEBUGSYS) {

        clr_addr = XO_PDN_TOP_CLRD0;
    }else if (clock_id <= HAL_CLOCK_CG_USB_DMA) {
        clr_addr = PDN_PD_CLRD0;
    }else if(clock_id == HAL_CLOCK_CG_CMSYS_ROM){
        clr_addr = PDN_TOP_CLRD0;
    }else {
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
    *(clr_addr) |= (0x1 << bit_idx);
#if !defined(__EXT_BOOTLOADER__) || defined(__EXT_DA__)
    if(clock_id != HAL_CLOCK_CG_END){
    const uint32_t *query = clk_linkage_tbl;//(uint32_t*)memmem(clk_linkage_tbl,sizeof(clk_linkage_tbl),&clock_id,4);
        for(;query <= &clk_linkage_tbl[CLK_AUD_INTERFACE1_SEL]; query++){
            if(*query == clock_id){
                clock_mux_sel_id linkage_mux = query - clk_linkage_tbl;
                ret = clock_mux_sel(linkage_mux,CLK_ENABLE);
                break;
            }
        }
    }
#endif

    return ret;
}

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_disable(hal_clock_cg_id clock_id)
{
    hal_clock_status_t ret = HAL_CLOCK_STATUS_OK;
    volatile uint32_t *set_addr = NULL;
    uint32_t bit_idx = clock_id & 0x1f;

    if(!hal_clock_is_enabled(clock_id))
        return ret;

    if (clock_id <= HAL_CLOCK_CG_AUXADC) {
        set_addr = XO_PDN_PD_SET0;
    } else if (clock_id <= HAL_CLOCK_CG_GPTIMER) {
        set_addr = XO_PDN_AO_SET0;
    } else if (clock_id <= HAL_CLOCK_CG_DEBUGSYS) {
        if(clock_id == HAL_CLOCK_CG_AUD_INTBUS){
            return HAL_CLOCK_STATUS_OK;
        }
        set_addr = XO_PDN_TOP_SET0;
    }else if (clock_id <= HAL_CLOCK_CG_USB_DMA) {
        set_addr = PDN_PD_SET0;
    }else if(clock_id == HAL_CLOCK_CG_CMSYS_ROM){
        set_addr = PDN_TOP_SET0;
    }else {
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
#if !defined(__EXT_BOOTLOADER__) || defined(__EXT_DA__)
if(clock_id != HAL_CLOCK_CG_END){
    const uint32_t *query = clk_linkage_tbl;//(uint32_t*)memmem(clk_linkage_tbl,sizeof(clk_linkage_tbl),&clock_id,4);
    for(;query <= &clk_linkage_tbl[CLK_AUD_INTERFACE1_SEL]; query++){
        if(*query == clock_id){
            clock_mux_sel_id linkage_mux = query - clk_linkage_tbl;
            ret = clock_mux_sel(linkage_mux,CLK_DISABLE);
            break;
        }
    }
}
#endif
    *(set_addr) |= (0x1 << bit_idx);

    return ret;
}


ATTR_TEXT_IN_TCM bool hal_clock_is_enabled(hal_clock_cg_id clock_id)
{
    uint32_t bit_idx = clock_id & 0x1f; // clock_id %32
    volatile uint32_t * sta_addr = NULL;

    if (clock_id <= HAL_CLOCK_CG_AUXADC) {
        sta_addr = XO_PDN_PD_COND0;
    } else if (clock_id <= HAL_CLOCK_CG_GPTIMER) {
        sta_addr = XO_PDN_AO_COND0;
    } else if (clock_id <= HAL_CLOCK_CG_DEBUGSYS) {
        sta_addr = XO_PDN_TOP_COND0;
    }else if (clock_id <= HAL_CLOCK_CG_USB_DMA) {
        sta_addr = PDN_PD_COND0;
    }else if(clock_id == HAL_CLOCK_CG_CMSYS_ROM){
        sta_addr = PDN_TOP_COND0;
    }else {
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }

    if (((*sta_addr) & (0x1 << bit_idx)) != 0x0) {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
        log_hal_msgid_info("%d: bit = %d: clock is disabled\n", 2,clock_id, bit_idx);
#endif
        return false;
    } else {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
         log_hal_msgid_info("%d: bit = %d: clock is enabled\n", 2,clock_id, bit_idx);
#endif
        return true;
    }
}

hal_clock_status_t hal_clock_init(void)
{

    hal_clock_enable(HAL_CLOCK_CG_DSP);
    hal_clock_enable(HAL_CLOCK_CG_CM4_DMA);
    hal_clock_disable(HAL_CLOCK_CG_ESC);
    hal_clock_disable(HAL_CLOCK_CG_AUXADC);
    hal_clock_disable(HAL_CLOCK_CG_I2C_AO);
    hal_clock_disable(HAL_CLOCK_CG_GPTIMER);
    //hal_clock_disable(HAL_CLOCK_CG_DEBUGSYS);
    hal_clock_disable(HAL_CLOCK_CG_SEJ);

    hal_clock_disable(HAL_CLOCK_CG_SPISLV);
    hal_clock_disable(HAL_CLOCK_CG_MIXEDSYS);
    hal_clock_disable(HAL_CLOCK_CG_EFUSE);
    hal_clock_disable(HAL_CLOCK_CG_AESOTF);
    hal_clock_disable(HAL_CLOCK_CG_AESOTF_ESC);
    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_clock_disable(HAL_CLOCK_CG_TRNG);
    hal_clock_disable(HAL_CLOCK_CG_SPISLV_BUS);
    hal_clock_disable(HAL_CLOCK_CG_SDIOMST0);
#ifndef __EXT_DA__
    hal_clock_disable(HAL_CLOCK_CG_USB);
    hal_clock_disable(HAL_CLOCK_CG_USB_BUS);
    hal_clock_disable(HAL_CLOCK_CG_USB_DMA);
#endif
    hal_clock_disable(HAL_CLOCK_CG_CMSYS_ROM);

    return clk_mux_init();
}
#endif /* HAL_CLOCK_MODULE_ENABLED */



