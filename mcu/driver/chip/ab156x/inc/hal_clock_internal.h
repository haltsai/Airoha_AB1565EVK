/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights RESERVED.
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

#ifndef __HAL_CLOCK_INTERNAL_H__
#define __HAL_CLOCK_INTERNAL_H__

#define BRING_UP
#define FPGA_VERIFY
#include "hal_clock.h"
#include "memory_attribute.h"
#ifdef HAL_CLOCK_MODULE_ENABLED
#include "hal_clock_platform.h"

#define CM4_DCM_CON_REG           ((cm4_dcm_con*)CM4_DCM_CON_0 )->field
#define SFC_DCM_CON_REG           ((sfc_dcm_con*)SFC_DCM_CON_0) ->field
#define ESC_DCM_CON_REG           ((esc_dcm_con*)ESC_DCM_CON_0) ->field
#define GPIO_CLKO_CTRL_A_REG      ((gpio_clko_ctrl_a*)GPIO_CLKO_CTRL_A) ->field
#define PLL_ABIST_FQMTR_COM_REG   ((pll_abist_fqmtr_con*)PLL_ABIST_FQMTR_CON0) ->field
#define CKSYS_MISC_REG            ((cksys_misc*)CKSYS_MISC_0) ->field
#define AUD_CKDIV_REG             ((aud_ckdiv_cfg*)RG_AUD_CKDIV_CFG0)->field

#define CKSYS_CLK_CFG_REG                ((cksys_clk_cfg*)CKSYS_CLK_CFG_0)->field
#define CKSYS_CLK_UPDATE_REG             ((cksys_clk_update*)CKSYS_CLK_UPDATE_0)->field
#define CKSYS_CLK_UPDATE_STATUS_REG      ((cksys_clk_update_status*)CKSYS_CLK_UPDATE_STATUS_0) ->field
#define CKSYS_CLK_FORCE_ON_REG           ((cksys_clk_force_on*)CKSYS_CLK_FORCE_ON_0) ->field
#define XO_DCM_CON_REG                   ((xo_dcm_con*)XO_DCM_CON_0)->field

#define CKSYS_CLK_DIV_REG                ((cksys_clk_div*)CKSYS_CLK_DIV_0) ->field
#define CKSYS_SRC_CLK_CG_REG             ((cksys_src_clk_cg*)CKSYS_SRC_CLK_CG0) ->field

#define CLKSQ_CON_REG                    ((clksq_con*)CLKSQ_CON0)->field
#define LPOSC_CON_REG                    ((lposc_con*)LPOSC_CON0)->field
#define APLL1_CON_REG                    ((apll_con*)APLL1_CTL0) ->field
#define APLL2_CON_REG                    ((apll_con*)APLL2_CTL0) ->field
#define UPLL_CON_REG                     ((upll_con*)UPLL_CTL0)  ->field
#define INFRA_CG_MTCMOS_REG              ((infra_cg_mtcmos*)INFRA_CG_MTCMOS3) ->field
#define SSC_CON                          ((ssc_con*)SSC_CON0) ->field

#define DCXO_PCON_REG                    ((dcxo_pcon*)DCXO_PCON0) ->field
#define DCXO_DEBUG_REG                   ((dcxo_debug*)DCXO_DEBUG0) ->field
#define DCXO_CORE_ISEL_LPM_REG           ((dcxo_core_isel_lpm*)DCXO_CORE_ISEL_LPM) ->field
#define CLOCK_SEL_REG                    ((clock_sel_con*)CLOCK_SEL0) ->field



#define BUS_DCM_CON_REG                  ((bus_dcm_con*)BUS_DCM_CON_0)->field
#define XO_DCM_CON_REG                   ((xo_dcm_con*)XO_DCM_CON_0)->field
#define MUX_SEL_NUM                      8
#define TUNE_TBL_SIZE                    2
#define NUM_DIV_SRC                      2

#define DCXO_DOMAIN_USR_EXIST            (clock_domain.dcxo_domain.src_cnt[0] || clock_domain.dcxo_domain.src_cnt[1]? exist : nonexist)
#define APLL1_DOMAIN_USR_EXIST           (clock_domain.dcxo_domain.field.free.field.apll1 ? exist : nonexist)
#define APLL2_DOMAIN_USR_EXIST           (clock_domain.dcxo_domain.field.free.field.apll2 ? exist : nonexist)
#define UPLL_DOMAIN_USR_EXIST            ((clock_domain.dcxo_domain.field.upll.src_cnt || clock_domain.dcxo_domain.field.free.field.upll) ? exist : nonexist)

#define LPOSC_DOMAIN_USR_EXIST           (clock_domain.lposc_domain.src_cnt ? exist : nonexist)
#define DCXO_DIR_USR_EXIST               (clock_domain.dcxo_domain.field.free.field.dcxo ?  exist : nonexist)

#define LPOSC_DOMAIN_USR_STATUS          &(clock_domain.lposc_domain.src_cnt)


#define TOPGEN_CLK_DOMAIN(mux_id)        (!(mux_id & 0x20))
#define PERIPHERAL_CLK_DOMAIN(mux_id)    (mux_id & 0x20)
#define DMA_ACTIVE                       (*SW_DMA_ACT & 0x5)


extern unsigned int dcxo_capid;
void hal_dcxo_init();
typedef enum {
    CLK_APLL1,
    CLK_APLL2,
    NR_APLL,
    CLK_UPLL = 2,
    NR_PLLS,
    CLK_OSC,
    CLK_XO,
    CLK_GPLL, // for build pass
} clock_pll_id;

typedef struct {
    clock_pll_id src;
    uint8_t div;
}pin_mux_t ;
typedef void (*post_handle)();

typedef enum{
    FINE_FT,
    COARSE_CALI,
    COARSE_FT,
    FINE_CALI,
}osc_cali_t;

typedef enum {
    lposc_cali_scale = 0x3F,
    lposc_ft_scale = 0x1F
}lposc_cali_t;

typedef struct {
    lposc_cali_t scale;
    hal_src_clock src;
    uint32_t cali_target;
}cali_info_t ;

typedef enum{
    CLK_DISABLE= 0xff,
    CLK_ENABLE= 0xfe,
}clock_linkage_type;

typedef enum {
    clk_dsp_dcm,
    clk_aud_bus_dcm,
    clk_bus_dcm,
    clk_sfc_dcm,
    clk_cm4_dcm,
    clk_esc_dcm
}hal_clk_dcm_id;

typedef enum{
    XO_CLK_26M   = 26000,
    OSC_CLK_139M = 138670,
    OSC_CLK_416M = 416000
}osc_clk_target_freq;


typedef enum {
    VOLT_0P73_LV,
    VOLT_0P75_LV,
    VOLT_0P8_LV,
    VOLT_0P9_LV,
    VOLT_LV_NUM,
    VOLT_NONE_LV
}hal_clock_volt_lv;

typedef enum {
    XO_CK,                 //26m/24m
    RTC_CK,                //32k
    PAD_SOC_REF_CK,            //NA
    CHOP_CK,               //1.3.20
    EOSC_F32K,             //NA
    DCXO_F32K,             //NA
    XOSC_F32K,             //NA
}ref_clock;

typedef enum{
    decr,
    incr
}mod_type;

typedef enum{
    nonexist,
    exist,
}clk_usr_status;

typedef enum{
    CLK_APLL_45M,
    CLK_APLL_49M
}clk_apll_freq;

typedef enum{
    DCXO_LP_MODE,
    DCXO_NORMAL_MODE,
}dcxo_mode_t ;

hal_clock_status_t clk_set_apll(clock_pll_id,clk_apll_freq);
typedef clk_usr_status (*usr_status)();

typedef struct {
   usr_status  usr_sta;
   post_handle post_hdr;
}clk_src_handler ;

typedef enum{
    UPLL_DIV_IDX,
    OSC_DIV_IDX,
    NONE_DIV
}clock_div_ctrl_idx;

typedef union {
    uint32_t src_cnt;
    struct {
        uint8_t div_low;
        uint8_t div_middle;
        uint8_t div_high;
        uint8_t div_max;
    }__attribute__((packed)) field;
}clk_div_info;

typedef union {
    uint32_t src_cnt;
    struct {
        uint8_t apll1;
        uint8_t apll2;
        uint8_t dcxo;
        uint8_t upll;
    }__attribute__((packed)) field;
}clk_dcxo_non_div_info;

typedef union{
    uint32_t src_cnt[2];
    struct {
        clk_div_info upll;
        clk_dcxo_non_div_info free;
    }__attribute__((packed)) field;
}clk_dcxo_domain;

typedef union{
    uint64_t src_cnt;
    struct {
        uint32_t free;
        clk_div_info osc;
    }__attribute__((packed)) field;
}clk_lposc_domain;

typedef enum{
    DVFS_DSP_CLOCK_83M = 5,
    DVFS_DSP_CLOCK_104M = 4,
    DVFS_DSP_CLOCK_208M = 3,
    DVFS_DSP_CLOCK_416M = 2,
}dvfs_clock_sys_sel;

typedef union{
    uint32_t value;
    struct {
        uint8_t sys;
        uint8_t dsp;
        uint8_t sfc;
        uint8_t esc;
    }field;
}clk_volt_lv_info ;

typedef enum{
    CLK_OSC_FREE,
    CLK_OSC_DIV
}osc_domain_indx;

ATTR_TEXT_IN_TCM hal_clock_status_t clock_div_ctrl(pin_mux_t mux,bool on_off);

typedef enum {
    CLK_DIV_D2 = 2,
    CLK_DIV_D3 = 3,
    CLK_DIV_D5 = 5,
    CLK_DIV_D7 = 7,
    CLK_DIV_D11 = 11,
    CLK_DIV_D15 = 15,
    CLK_DIV_RESERVED = 0xfe,
    CLK_DIV_NONE
}AG3335_DIV_TYPE;


typedef struct {
    volatile pin_mux_t *cur_sel_0;
    volatile pin_mux_t *cur_sel_1;
    volatile pin_mux_t *cur_sel_2;
    volatile pin_mux_t *cur_sel_3;
    volatile pin_mux_t *cur_sel_s;
    pin_mux_t sels[MUX_SEL_NUM];
}top_mux_info;

typedef struct {
    clk_div_info div_tbl[NUM_DIV_SRC];
}clk_top_div_info;

typedef struct {
    top_mux_info               top_mux_info_list[NR_MUXS];
    top_mux_info               peri_mux_info_list[(CLK_PERI_NUM &0x1f)];
    clk_top_div_info           div_info;
    volatile clk_dcxo_domain   dcxo_domain;
    volatile clk_lposc_domain  lposc_domain;
}clock_domain_t;


typedef union {
    uint32_t value[8];
    struct {
        __IO uint8_t GSM_DCXO_CTL_EN;
        __IO uint8_t DIS_DCXO_ISO_EN;
        __IO uint8_t FRC_BB26MCK_OFF;
        __IO uint8_t FRC_DCXO_PWR_ON;//0
        __IO uint8_t FRC_DCXO_F26M_RDY;
        __IO uint8_t FRC_BB26MCK_EN;
        __IO uint8_t FRC_COCLK_EN;
        __IO uint8_t EXT_DCXO_CTL_EN;//1
        __IO uint8_t FRC_EXT_SGLSTGE;
        __IO uint8_t HW_COCLK_EN;
        __IO uint16_t FRC_GSM_SGLSTGE;//2
        __IO uint8_t DCXO_CON_RSV;
        __IO uint8_t DCXO_ISO_EN_TD;
        __IO uint8_t DCXO_BUF_EN_TD;
        __IO uint8_t DCXO_EN_TD;//3
        __IO uint16_t DCXO_PWR_EN_TD;
        __IO uint8_t DCXO_SLEEP_TD;
        __IO uint8_t DCXO_26M_RDY_EN;//4
        __IO uint8_t EN_26M_FPM;
        __IO uint8_t BT_26M_EN;
        __IO uint16_t DCXO32K_EN;//5
        __IO uint16_t DCXO_CAPID_EFUSE_SEL;
        __IO uint16_t DCXO_CAPID_EFUSE;//6
        __IO uint8_t DCXO_CK_RDY_COMP_VREF_SEL;
        __IO uint8_t DCXO_CK_RDY_COMP_VREF_DBB;
        __IO uint8_t RG_DCXO_FPM_LDO_OFF_PULLDOWN;
        __IO uint8_t HI_CDAC_IBOOST_EN;//7
    }field;
}dcxo_pcon;

typedef union {
    uint32_t value[4];
    struct {
        __IO uint8_t CLK_PLL1_DIV_EN;
        __IO uint8_t CLK_OSC1_DIV_EN;
        uint16_t RESERVED_1;
        __IO uint8_t CLK_PLL1_D2_EN;
        __IO uint8_t CLK_PLL1_D3_EN;
        __IO uint8_t CLK_PLL1_D5_EN;
        uint8_t RESERVED_2;
        __IO uint8_t CLK_OSC1_D2_EN;
        __IO uint8_t CLK_OSC1_D3_EN;
        __IO uint8_t CLK_OSC1_D5_EN;
        uint8_t RESERVED_3;
        __IO uint8_t CHOP_DIV_EN;
        __IO uint8_t CHOP_DIV_CHG;
        __IO uint16_t CHOP_DIV_SEL;
    }field;
}cksys_clk_div;

typedef union {
    uint32_t value;
    struct {
        __IO uint8_t RESERVED;
        __IO uint8_t ISEL_LPM:4;
        __IO uint8_t RESERVED_1:4;
        __IO uint16_t RESERVED_2;
    }field;
}dcxo_core_isel_lpm ;

typedef union {
    uint32_t value;
    struct {
        uint8_t CLK_MODE0;
        uint8_t CLK_MODE1;
        uint8_t CLK_MODE2;
        uint8_t CLK_MODE3;
    }__attribute__((packed)) field;
}gpio_clko_ctrl_a;


typedef union{
    uint64_t value;
    struct {
        __IO uint16_t DCXO_DEBUG_EN;
        __IO uint16_t DCXO_DEBUG_SEL;
        __O uint16_t DCXO_DEBUG_O;
        uint16_t reserved;
    } field;
}dcxo_debug;

typedef union {
    uint64_t value;
    struct {
        __IO uint8_t RG_CM_SFSEL; // 1bit
        __IO uint8_t RG_CM_DCM_DBC_NUM; // 1bit
        __IO uint8_t RG_CM_DCM_DBC_EN; // 1bit
        uint8_t RESERVED_1;
        uint8_t RESERVED_2;
        __IO uint8_t RG_CM_CLKSLOW_EN; // 1bit
        __IO uint8_t RG_CM_FORCE_CLKOFF; // 1bit
        __IO uint8_t RG_CM_FORCE_CLKSLOW; // 1bit
    } field;
}cm4_dcm_con;


typedef union {
    uint64_t value;
    struct {
        __IO uint8_t RG_XO_SFSEL; // 1bit
        __IO uint8_t RG_XO_DCM_DBC_NUM; // 1bit
        __IO uint8_t RG_XO_DCM_DBC_EN; // 1bit
        __IO uint8_t RG_XO_DCM_EN;
        __IO uint8_t RG_XO_CLKOFF_EN; // 1bit
        __IO uint8_t RG_XO_CLKSLOW_EN; // 1bit
        __IO uint8_t RG_XO_FORCE_CLKOFF; // 1bit
        __IO uint8_t RG_XO_FORCE_CLKSLOW; // 1bit
    } field;
}xo_dcm_con;

typedef union {
    uint64_t value;
    struct {
        uint8_t RESERVED_0;
        __IO uint8_t RG_SFC_DCM_DBC_NUM;
        __IO uint8_t RG_SFC_DCM_DBC_EN;
        __IO uint8_t RG_SFC_DCM_APB_SEL;
        __IO uint8_t RG_SFC_CLK_OFF_EN;
        uint8_t RESERVED_1;
        __IO uint8_t RG_SFC_FORCE_CLK_OFF;
        __IO uint8_t RG_SFC_DCM_APB_TOG;
    } field;
}sfc_dcm_con;

typedef union {
    uint64_t value;
    struct {
        uint8_t RESERVED_0;
        __IO uint8_t RG_ESC_DCM_DBC_NUM;
        __IO uint8_t RG_ESC_DCM_DBC_EN;
        __IO uint8_t RG_ESC_DCM_APB_SEL;
        __IO uint8_t RG_ESC_CLK_OFF_EN;
        uint8_t RESERVED_1;
        __IO uint8_t RG_ESC_FORCE_CLK_OFF;
        __IO uint8_t RG_ESC_DCM_APB_TOG;
    } field;
}esc_dcm_con;


typedef union {
    uint64_t value;
    struct {
        __IO uint8_t OSC_CKSYS_EN;
        __IO uint8_t OSC_CKDIV_EN;
        uint16_t RESERVED;
        __IO uint8_t PLL1_CKSYS_EN;
        __IO uint8_t PLL1_CKDIV_EN;
        uint16_t RESERVED_1;
    } field;
}cksys_src_clk_cg;

typedef union {
    uint32_t value[11];
    struct {
        __IO uint8_t DA_UPLL_EN;
        __IO uint8_t RG_UPLL_PREDIV;
        __IO uint16_t RG_UPLL_POSTDIV;
        __IO uint8_t RG_UPLL_FBDIV;
        __IO uint8_t RG_UPLL_FBDIV2;
        __IO uint16_t RG_UPLL_48M_EN;
        uint32_t RESERVED_CON2;
        uint32_t RESERVED_CON3;
        uint32_t RESERVED_CON4;
        uint32_t RESERVED_CON5;
        uint16_t RESERVED_CON6;
        __IO uint16_t RG_UPLL_VTMON_EN ;
        __IO uint16_t RG_UPLL_IBANK_EN;
        __IO uint16_t RG_UPLL_IBANK_FINETUNE;
        __IO uint32_t RG_UPLL_GLOBAL_LDO_BIAS_EN;
        uint8_t RESERVED_CON9;
        __IO uint8_t RG_UPLL_CP_EN;
        uint16_t RESERVED_CON9_1;
        __IO uint16_t RG_UPLL_KVCO_SEL;
        uint16_t RESERVED_CON10;
    } field;
}upll_con;

typedef union {
    uint32_t value[5];
    struct {
        __IO uint32_t BP_PLL_DLY;
        __IO uint8_t UPLL_EN_SEL;
        __IO uint8_t UPLL_CK_CG;
        __IO uint8_t UPLL_FORCE_ON;
        __IO uint8_t RG_UPLL_RDY;
        __IO uint8_t LPOSC_EN_SEL;
        __IO uint8_t LPOSC_CK_CG;
        __IO uint8_t LPOSC_FORCE_ON;
        __IO uint8_t RG_LPOSC_RDY;
        __IO uint16_t UPLL_START_TIME;
        __IO uint16_t UPLL_SETTLE_TIME;
        __IO uint16_t LPOSC_START_TIME;
        __IO uint16_t LPOSC_SETTLE_TIME;
    } field;
}clksq_con;

typedef union {
    uint32_t value[17];
    struct {
        __IO uint8_t RG_APLL1_DDS_PWR_ON;
        __IO uint8_t RG_APLL1_DDS_ISO_EN;
        __IO uint8_t AD_RGS_APLL1_DDS_PWR_ACK;
        __IO uint8_t RG_APLL1_V2I_EN;
        __IO uint8_t RG_APLL1_EN;
        __IO uint8_t RG_APLL1_DIV16_EN;
        __IO uint8_t RG_APLL1_POSDIV;
        __IO uint8_t RG_APLL1_PREDIV;
        __IO uint8_t RG_APLL1_FBKDIV;
        __IO uint8_t RG_APLL1_BR;
        __IO uint8_t RG_APLL1_BP;
        __IO uint8_t RG_APLL1_FBDIV_PREDIV2_EN;
        __IO uint8_t RG_APLL1_MONREF_EN;
        __IO uint8_t RG_APLL1_MONVC_EN;
        __IO uint8_t RG_APLL1_DIV;
        __IO uint8_t RG_APLL1_MONCK_EN;
        __IO uint8_t RG_APLL1_DDSFBK_EN;
        __IO uint8_t RG_APLL1_BP2X;
        __IO uint8_t RG_APLL1_LVROD_EN;
        __IO uint8_t RG_APLL1_RESET_DLY;
        uint16_t RESERVED;
        __IO uint8_t RG_APLL1_VREF_FC;
        __IO uint8_t RG_APLL1_CP_EN;
        __IO uint8_t RG_APLL1_KVCO_SEL;
        __IO uint8_t RG_APLL1_IBANK_FC;
        __IO uint8_t RG_APLL1_IBANK_FINETUNE;
        uint8_t RESERVED_1;
        __IO uint8_t RG_APLL1_VTMON_WINDOW_SEL;
        __IO uint8_t RG_APLL1_VTMON_EN;
        __IO uint8_t RG_APLL1_LOCKDET_WINDOW_SEL;
        __IO uint8_t RG_APLL1_LOCKDET_EN;
        __IO uint8_t RG_APLL1_TESTBUF_EN;
        __IO uint8_t RG_APLL1_CLKMUX_SEL;
        __IO uint8_t RG_APLL1_CP_ISEL;
        __IO uint8_t RG_APLL1_CLK_BYPASS_EN;
        __IO uint8_t RG_APLL1_RESERVE;
        __IO uint8_t RG_APLL1_TESTDIV2_EN;
        __IO uint8_t RG_APLL1_LCDDS_LVROD_EN;
        __IO uint8_t RG_APLL1_TESTBUF_DRIVING_SEL;
        __IO uint32_t RG_APLL1_LCDDS_PCW_NCPO;//[10]
        __IO uint8_t RG_APLL1_LCDDS_EN;
        __IO uint8_t RG_APLL1_LCDDS_MON_EN;
        __IO uint8_t RG_APLL1_LCDDS_C;
        __IO uint8_t RG_APLL1_LCDDS_PREDIV2;
        __IO uint8_t RG_APLL1_LCDDS_PCW_NCPO_CHG;
        __IO uint8_t RG_APLL1_LCDDS_SSC_EN;
        __IO uint8_t RG_APLL1_DIVSEL;
        uint8_t RESERVED_2;
        __IO uint32_t RG_APLL1_LCDDS_TUNER_EN;
        __IO uint32_t RG_APLL1_LCDDS_TUNER_PCW_NCPO; //[14]
        __IO uint32_t DA_APLL1_LCDDS_PCW_NCPO;
        uint16_t RESERVED_3;
        __IO uint16_t RG_APLL1_IBANK_EN;
    } field;
}apll_con;

typedef union {
    uint64_t value[4];
    struct {
        __IO uint8_t APLL12_DIV0_PDN;
        __IO uint8_t APLL12_DIV1_PDN;
        __IO uint8_t APLL12_DIV2_PDN;
        __IO uint8_t APLL12_DIV3_PDN;
        __IO uint8_t APLL_I2S0_MCK_SEL;
        __IO uint8_t APLL_I2S1_MCK_SEL;
        __IO uint8_t APLL_I2S2_MCK_SEL;
        __IO uint8_t APLL_I2S3_MCK_SEL;
        __IO uint8_t APLL12_CK_DIV0;
        __IO uint8_t APLL12_CK_DIV0_CHG;
        __IO uint8_t APLL12_CK_DIV1;
        __IO uint8_t APLL12_CK_DIV1_CHG;
        __IO uint8_t APLL12_CK_DIV2;
        __IO uint8_t APLL12_CK_DIV2_CHG;
        __IO uint8_t APLL12_CK_DIV3;
        __IO uint8_t APLL12_CK_DIV3_CHG;
    } field;
}aud_ckdiv_cfg;

typedef union {
    uint32_t value;
    struct {
        __IO uint8_t DA_HFOSC_EN;
        __IO uint8_t RG_HFOSC_160M_EN;
        __IO uint8_t RG_HFOSC_FT;
        __IO uint8_t RG_HFOSC_CALI;
        uint16_t RESERVED;
        __IO uint8_t RG_HFOSC_BIAS;
    } field;
}lposc_con;

typedef union {
    uint64_t value;
    struct {
        __IO uint8_t RG_BUS_SFSEL;
        __IO uint8_t RG_BUS_DCM_DBC_NUM;
        __IO uint8_t RG_BUS_DCM_DBC_EN;
        __IO uint8_t RG_BUS_DCM_EN;
        __IO uint8_t RG_BUS_CLKOFF_EN;
        __IO uint8_t RG_BUS_CLKSLOW_EN;
        __IO uint8_t RG_BUS_FORCE_CLK_OFF;
        __IO uint8_t RG_BUS_FORCE_CLKSLOW;
    } field;
}bus_dcm_con;

typedef union { 
    uint64_t value;
    struct {
        __IO uint8_t RG_UART0_BCLK_SEL;
        __IO uint8_t RG_UART1_BCLK_SEL;
        uint16_t RESERVED;
        __IO uint8_t RG_GPT_BCLK_SEL;
        __IO uint8_t RG_OSGPT_BCLK_SEL;
        uint16_t RESERVED_1;
    } field;
}clock_sel_con;

typedef union {
    uint32_t value[3];
    struct {
        __IO uint32_t FQMTR_WINSET;
        __IO uint16_t PLL_ABIST_FQMTR_CON1_; //FQMTR_BUSY Frequency-Meter busy +status
        __IO uint8_t FQMTR_EN;
        __IO uint8_t FQMTR_RST;
        __IO uint8_t FQMTR_CLKDIV;
        __IO uint8_t FQMTR_CLKDIV_EN;
        uint16_t RESERVED;
    } field;
}pll_abist_fqmtr_con;

typedef union {
    uint32_t value[5];
    struct {
        __IO uint8_t CLK_SYS_SEL;
        __IO uint8_t CLK_DSP_SEL;
        __IO uint8_t CLK_SFC_SEL;
        __IO uint8_t CLK_ESC_SEL;
        __IO uint8_t CLK_SPIMST0_SEL;
        __IO uint8_t CLK_SPIMST1_SEL;
        __IO uint8_t CLK_SPIMST2_SEL;
        __IO uint8_t CLK_SPISLV_SEL;
        __IO uint8_t CLK_SDIOMST0_SEL;
        __IO uint8_t CLK_USB_SEL;
        __IO uint8_t CLK_AUD_BUS_SEL;
        __IO uint8_t CLK_AUD_GPSRC_SEL;
        __IO uint8_t CLK_AUD_ULCK_SEL;
        __IO uint8_t CLK_AUD_DLCK_SEL;
        __IO uint8_t CLK_26M_SEL;
        uint8_t RESERVED_2;
        __IO uint8_t CLK_AUD_INTF0_SEL;
        __IO uint8_t CLK_AUD_INTF1_SEL;
        __IO uint8_t CLK_AUD_ENGINE_SEL;
        __IO uint8_t CLK_VOW_SEL;
    } field;
}cksys_clk_cfg;


typedef union {
    uint32_t value[4];
    struct {
        __IO uint8_t CHG_SYS;
        __IO uint8_t CHG_DSP;
        __IO uint8_t CHG_SFC;
        __IO uint8_t CHG_ESC;
        __IO uint8_t CHG_SPIMST0;
        __IO uint8_t CHG_SPIMST1;
        __IO uint8_t CHG_SPIMST2;
        __IO uint8_t CHG_SPISLV;
        __IO uint8_t CHG_SDIOMST0;
        __IO uint8_t CHG_USB;
        __IO uint8_t CHG_AUD_BUS;
        __IO uint8_t CHG_AUD_GPSRC;
        __IO uint8_t CHG_AUD_ULCK;
        __IO uint8_t CHG_AUD_DLCK;
        uint16_t RESERVED_2;
    } field;
}cksys_clk_update;

typedef union {
    uint32_t value[4];
    struct {
        __IO uint8_t CLK_SYS_OK;
        __IO uint8_t CLK_DSP_OK;
        __IO uint8_t CLK_SFC_OK;
        __IO uint8_t CLK_ESC_OK;
        __IO uint8_t CLK_SPIMST0_OK;
        __IO uint8_t CLK_SPIMST1_OK;
        __IO uint8_t CLK_SPIMST2_OK;
        __IO uint8_t CLK_SPISLV_OK;
        __IO uint8_t CLK_SDIOMST0_OK;
        __IO uint8_t CLK_USB_OK;
        __IO uint8_t CLK_AUD_BUS_OK;
        __IO uint8_t CLK_AUD_GPSRC_OK;
        __IO uint8_t CLK_AUD_ULCK_OK;
        __IO uint8_t CLK_AUD_DLCK_OK;
        uint16_t RESERVED_2;
    } field;
}cksys_clk_update_status;

typedef union {
    uint32_t value[4];
    struct {
        __IO uint8_t CLK_SYS_FORCE_ON;
        __IO uint8_t CLK_DSP_FORCE_ON;
        __IO uint8_t CLK_SFC_FORCE_ON;
        __IO uint8_t CLK_ESC_FORCE_ON;
        __IO uint8_t CLK_SPIMST0_FORCE_ON;
        __IO uint8_t CLK_SPIMST1_FORCE_ON;
        __IO uint8_t CLK_SPIMST2_FORCE_ON;
        __IO uint8_t CLK_SPISLV_FORCE_ON;
        __IO uint8_t CLK_SDIOMST0_FORCE_ON;
        __IO uint8_t CLK_USB_FORCE_ON;
        __IO uint8_t CLK_AUD_BUS_FORCE_ON;
        __IO uint8_t CLK_AUD_GPSRC_FORCE_ON;
        __IO uint8_t CLK_AUD_ULCK_FORCE_ON;
        __IO uint8_t CLK_AUD_DLCK_FORCE_ON;
        uint16_t RESERVED_2;
    } field;
}cksys_clk_force_on;


typedef union {
    uint32_t value[5];
    struct {
        __IO uint8_t HFOSC_FHCTL_EN;
        __IO uint8_t HFOSC_SFSTR_EN;
        __IO uint8_t HFOSC_FRSSC_EN;
        __IO uint8_t HFOSC_FHCTL_RST;
        __IO uint8_t HFOSC_SFSTR_DTS;
        __IO uint8_t HFOSC_SFSTR_DYS;
        __IO uint8_t HFOSC_FRSSC_DTS;
        __IO uint8_t HFOSC_FRSSC_DYS;
        __I uint8_t HFOSC_FHCTL_STATE;
        __I uint8_t FRSSC_HFOSC_PRD;
        __I uint8_t SFSTR_HFOSC_PRD;
        uint8_t RESERVED ;
        __IO uint16_t HFOSC_FRSSC_UPLMT;
        __IO uint16_t HFOSC_FRSSC_DNLMT;
        __IO uint16_t HFOSC_FREQ_STR;
        __IO uint16_t HFOSC_FHSET;
    } field;
}ssc_con;
typedef union {
    uint32_t value;
    struct {
        __IO uint8_t AUDIO_PROT_EN;
        __IO uint8_t AUDIO_PROT_RDY;
        uint16_t RESERVED;
    } field;
}infra_cg_mtcmos;
void dvfs_switch_clock_freq(hal_clock_volt_lv cur_lv,hal_clock_volt_lv next_lv);
uint8_t clock_mux_cur_sel(clock_mux_sel_id mux_id);
void clock_dump_info(void);
void chop_clock_sel(uint32_t);
void hal_clock_set_pll_dcm_init();
ATTR_TEXT_IN_TCM hal_clock_status_t clock_mux_sel(clock_mux_sel_id mux_id, uint32_t mux_sel);
/*************************************************************************
 * Define customized function prototype
 *************************************************************************/

#endif /* HAL_CLOCK_MODULE_ENABLED */
#endif /* __HAL_CLOCK_INTERNAL_H__ */
