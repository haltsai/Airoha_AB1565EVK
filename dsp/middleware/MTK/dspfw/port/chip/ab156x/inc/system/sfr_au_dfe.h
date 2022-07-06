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
#ifndef _AU_DFE_SFR_H_
#define _AU_DFE_SFR_H_
#include "types.h"


/******************************************************************************
 * Enumerations
 ******************************************************************************/
typedef enum
{
    reserved0,
    // AU_DEF_UP_BY_1      = 0,
    // AU_DEF_UP_BY_2      = 1,
    // AU_DEF_UP_BY_3      = 2,
    // AU_DEF_UP_BY_4      = 3,
    // AU_DEF_UP_BY_6      = 4,
    // AU_DEF_UP_BY_12     = 5,

    // AU_DEF_DOWN_BY_1    = 0,
    // AU_DEF_DOWN_BY_2    = 1,
    // AU_DEF_DOWN_BY_3    = 2,
    // AU_DEF_DOWN_BY_4    = 3,
    // AU_DEF_DOWN_BY_6    = 4,
    // AU_DEF_DOWN_BY_12   = 5,

}AU_DFE_UPDOWN_RATIO;


typedef enum
{
    reserved1,
    // ODFE_RATE_ALIGN_DAC         = 0,
    // ODFE_RATE_ALIGN_I2S_MASTER  = 1,
    // ODFE_RATE_ALIGN_I2S_SLAVE   = 2,
    // ODFE_RATE_ALIGN_SPDIF_TX    = 3,
}AU_ODFE_THRP_RATE_SEL;

typedef enum
{
    reserved2,
    // IDFE_RATE_ALIGN_ADC         = 0,
    // IDFE_RATE_ALIGN_I2S_MASTER  = 1,
    // IDFE_RATE_ALIGN_I2S_SLAVE   = 2,
    // IDFE_RATE_ALIGN_SPDIF_RX    = 3,
    // IDFE_RATE_ALIGN_mDSP_CH0123 = 4,
    // IDFE_RATE_ALIGN_mDSP_CH45 = 5,
    // IDFE_RATE_ALIGN_mDSP_CH2301 = 6,
}AU_IDFE_THRP_RATE_SEL;


/******************************************************************************
 * AUDIO_DFE
 * Base Address = 0x42844000
 ******************************************************************************/

/* offset 0x0000 */
typedef union DFE_CTL0_u
{
    // struct DFE_CTL0_s
    // {
        // U32 EN_AU_ODFE_INT4	        : 1;
        // U32 SET_AU_ODFE_INT4_ACT_CH : 1;
        // U32 EN_AU_ODFE_INT6         : 1;
        // U32 EN_AU_DBG_CH            : 1;
        // U32 _RSVD_0_                : 3;
        // U32 EN_AU_ODFE_INTF         : 1;
        // U32 EN_AU_IDFE_DEC3         : 1;
        // U32 SET_AU_IDFE_DEC3_ACT_CH : 2;
		// U32 _RSVD_1_                : 1;
        // U32 EN_AU_IDFE_DEC9         : 1;
		// U32 _RSVD_2_                : 2;
        // U32 EN_AU_IDFE_INTF         : 1;
        // U32 _RSVD_3_                : 16;
    // } field;
    // U32 reg;
} DFE_CTL0_t;

/* offset 0x0004 */
typedef union DFE_CTL1_u
{
    // struct DFE_CTL1_s
    // {
        // U32 RST_AU_ODFE_INT4	    : 1;
        // U32 SET_AU_ODFE_INT4_ACT_CH : 1;
        // U32 RST_AU_ODFE_INT6        : 1;
        // U32 RST_AU_DBG_CH           : 1;
        // U32 _RSVD_0_                : 3;
        // U32 RST_AU_ODFE_INTF        : 1;
        // U32 RST_AU_IDFE_DEC3        : 1;
        // U32 SET_AU_IDFE_DEC3_ACT_CH : 2;
		// U32 _RSVD_1_                : 1;
        // U32 RST_AU_IDFE_DEC9        : 1;
		// U32 _RSVD_2_                : 2;
        // U32 RST_AU_IDFE_INTF        : 1;
        // U32 _RSVD_3_                : 16;
    // } field;
    // U32 reg;
} DFE_CTL1_t;


typedef enum AU_ODFE_CLK_GATE_e
{
    reserved3,
    // ODFE_CLK_24MHZ      = 0,
    // ODFE_CLK_12MHZ      = 1,
    // ODFE_CLK_6MHZ       = 2,
    // ODFE_CLK_3MHZ       = 3,
    // ODFE_CLK_2_4MHZ     = 4,
    // ODFE_CLK_1_5MHZ     = 5,
    // ODFE_CLK_0_75MHZ    = 6,
    // ODFE_CLK_OFF        = 7,
} AU_ODFE_CLK_GATE_t;

typedef enum AU_IDFE_CLK_GATE_e
{
    reserved4,
    // IDFE_CLK_24MHZ      = 0,
    // IDFE_CLK_12MHZ      = 1,
    // IDFE_CLK_6MHZ       = 2,
    // IDFE_CLK_3MHZ       = 3,
    // IDFE_CLK_2_4MHZ     = 4,
    // IDFE_CLK_1_5MHZ     = 5,
    // IDFE_CLK_0_75MHZ    = 6,
    // IDFE_CLK_OFF        = 7,
} AU_IDFE_CLK_GATE_t;

/* offset 0x0008 */
typedef union DFE_CTL2_u
{
    // struct DFE_CTL2_s
    // {
        // U32 AU_ODFE_SIDETONE_GAIN	: 10;
        // U32 _RSVD_0_                : 14;
        // U32 AU_ODFE_GATED_CLK       : 3;
        // U32 ODFE_CLK_EN             : 1;
        // U32 AU_IDFE_GATED_CLK       : 3;
        // U32 IDFE_CLK_EN             : 1;


    // } field;
    // U32 reg;
} DFE_CTL2_t;

/* offset 0x000C */
typedef union ODFE_SET0_u
{
    // struct ODFE_SET0_s
    // {
        // AU_DFE_UPDOWN_RATIO     INT4_UPS_RATIO	        : 3;
        // U32                     INT4_BIT_RES            : 1;
        // U32                     INT4_IN_DATA_MUX        : 1;
        // U32                     BYPASS_INT4_CC1_FIL     : 1;
        // U32                     BYPASS_INT6_CC1_FIL     : 1;
        // U32                     _RSVD_0_                : 1;
        // AU_DFE_UPDOWN_RATIO     INT6_UPS_RATIO	        : 3;
        // U32                     INT6_BIT_RES            : 1;
        // AU_ODFE_THRP_RATE_SEL   ODFE_THRP_RATE_SEL      : 3;
        // U32                     _RSVD_1_                : 1;
        // U32                     I2S_MASTER_SDO_MUX      : 3;
        // U32                     I2S_SLAVE_SDO_MUX       : 3;
        // U32                     SPDIF_TX_MUX            : 3;
        // U32                     DBG_BUF_THRP_RATE_SEL   : 2;
        // U32                     _RSVD_2_                : 5;
    // } field;
    // U32 reg;
} ODFE_SET0_t;

/* offset 0x0010 */
typedef union IDFE_SET0_u
{
    // struct IDFE_SET0_s
    // {
        // AU_DFE_UPDOWN_RATIO     DEC3_DWN_RATIO	        : 3;
        // U32                     DEC3_BIT_RES            : 1;
        // U32                     _RSVD_0_                : 8;
        // AU_IDFE_THRP_RATE_SEL   IDFE_THRP_RATE_SEL      : 3;
        // U32                     _RSVD_1_                : 1;
        // AU_DFE_UPDOWN_RATIO     DEC9_DWN_RATIO          : 3;
        // U32                     DEC9_BIT_RES            : 1;
        // U32                     EC_PATH_DATA_SEL        : 2;
        // U32                     _RSVD_2_                : 2;
        // U32                     SIDETONE_SEL            : 2;
        // U32                     _RSVD_3_                : 6;
    // } field;
    // U32 reg;
} IDFE_SET0_t;

/* offset 0x0014 */
typedef union AU_DFE_TEST_u
{
    // struct AU_DFE_TEST_s
    // {
        // U32 OUT_TEST	            : 16;
        // U32 IN_TEST                 : 16;
    // } field;
    // U32 reg;
} AU_DFE_TEST_t;


/* offset 0x0018 */
typedef union AU_DFE_DBG_PORT_u
{
    // struct AU_DFE_DBG_PORT_s
    // {
        // U32 ODFE_DBG_PORT_SEL                : 8;
        // U32 IDFE_DBG_PORT_SEL                : 8;
        // U32 DFE_DBG_PORT_SEL                 : 7;
        // U32 IDFE_TEST_SETTING                : 9;
    // } field;
    // U32 reg;
} AU_DFE_DBG_PORT_t;





typedef struct
{
	// DFE_CTL0_t				        CTL0;		   	//offset 0x0000
    // DFE_CTL1_t				        CTL1;		   	//offset 0x0004
    // DFE_CTL2_t                      CTL2;		   	//offset 0x0008
    // ODFE_SET0_t                     OUT_SET0;       //offset 0x000C
    // IDFE_SET0_t                     IN_SET0;        //offset 0x0010
    // AU_DFE_TEST_t                   TEST_CSR;       //offset 0x0014
    // AU_DFE_DBG_PORT_t               DBG_SET;        //offset 0x0018
} AUDIO_DFE_s;

// EXTERN VOLATILE AUDIO_DFE_s AUDIO_DFE;      /* offset 0x42844000 */

/* offset 0x0110 */
typedef union ADMA_CTL_u
{
    // struct ADMA_CTL_s
    // {
        // U32 ENABLE	                : 1;
        // U32 THD_INTR_MASK           : 1;
        // U32 ERR_INTR_MASK           : 1;
        // U32 DATA_DIR                : 1;
        // U32 FLUSH_DATA              : 1;
        // U32 _RSVD_0_                : 3;
        // U32 SW_RESET                : 1;
        // U32 THD_INTR_CLR            : 1;
        // U32 ERR_INTR_CLR            : 1;
        // U32 START_ADDR_TRIG         : 1;
        // U32 _RSVD_1_                : 20;
    // } field;
    // U32 reg;
} ADMA_CTL_t;

/* offset 0x0114 */
typedef union ADMA_BTHD_BSIZE_u
{
    // struct ADMA_BTHD_BSIZE_s
    // {
        // U32 BUF_SIZE	            : 16;
        // U32 THD_SIZE                : 16;
    // } field;
    // U32 reg;
} ADMA_BTHD_BSIZE_t;

/* offset 0x0118 */
typedef union ADMA_INI_ADDR_u
{
    // struct ADMA_INI_ADDR_s
    // {
        // U32 ADDR;
    // } field;
    // U32 reg;
} ADMA_INI_ADDR_t;

/* offset 0x011C */
typedef union ADMA_STATUS_u
{
    // struct ADMA_STATUS_s
    // {
        // U32 INTR_TOKEN	            : 1;
        // U32 THD_INTR                : 1;
        // U32 ERR_INTR                : 1;
        // U32 _RSVD_0_                : 27;
        // U32 FSM                     : 2;
    // } field;
    // U32 reg;
} ADMA_STATUS_t;

/* offset 0x0120 */
typedef union ADMA_NEXT_ADDR_u
{
    // struct ADMA_NEXT_ADDR_s
    // {
        // U32 ADDR;
    // } field;
    // U32 reg;
} ADMA_NEXT_ADDR_t;

typedef struct
{
	// ADMA_CTL_t				        CTL;		   	    /* offset 0x0110 */
	// ADMA_BTHD_BSIZE_t               SET;                /* offset 0x0114 */
	// ADMA_INI_ADDR_t                 INIT;               /* offset 0x0118 */
	// ADMA_STATUS_t                   STAT;               /* offset 0x011C */
	// ADMA_NEXT_ADDR_t                NEXT;               /* offset 0x0120 */
} AU_ODFE_RADMA_s, *AU_ODFE_RADMA_PTR;

// EXTERN VOLATILE  AU_ODFE_RADMA_s    AU_ODFE_CH0_RADMA,  /* 0x42844110 */
                                    // AU_ODFE_CH1_RADMA,  /* 0x42844210 */
                                    // AU_ODFE_CH2_RADMA,  /* 0x42844310 */
                                    // AU_ODFE_CH3_RADMA,  /* 0x42844410 */
                                    // AU_ODFE_CH4_RADMA;  /* 0x42844510 */

typedef struct
{
	// ADMA_CTL_t				        CTL;		   	    /* offset 0x0610 */
	// ADMA_BTHD_BSIZE_t               SET;                /* offset 0x0614 */
	// ADMA_INI_ADDR_t                 INIT;               /* offset 0x0618 */
	// ADMA_STATUS_t                   STAT;               /* offset 0x061C */
	// ADMA_NEXT_ADDR_t                NEXT;               /* offset 0x0620 */
} AU_IDFE_WADMA_s, *AU_IDFE_WADMA_PTR;

// EXTERN VOLATILE AU_IDFE_WADMA_s     AU_IDFE_CH0_WADMA,  /* 0x42844610 */
                                    // AU_IDFE_CH1_WADMA,  /* 0x42844710 */
                                    // AU_IDFE_CH2_WADMA,  /* 0x42844810 */
                                    // AU_IDFE_CH3_WADMA,  /* 0x42844910 */
                                    // AU_IDFE_CH4_WADMA;  /* 0x42844A10 */


// EXTERN VOLATILE AU_ODFE_RADMA_s     AU_mDFE_CH5_RADMA,  /* 0x42844B10 */
                                    // AU_mDFE_CH6_RADMA;  /* 0x42844C10 */

// EXTERN VOLATILE AU_IDFE_WADMA_s     AU_mDFE_CH5_WADMA,  /* 0x42844D10 */
                                    // AU_mDFE_CH6_WADMA;  /* 0x42844E10 */


typedef struct
{
	// ADMA_CTL_t				        CTL;		   	    /* offset 0x0110 */
	// ADMA_BTHD_BSIZE_t               SET;                /* offset 0x0114 */
	// ADMA_INI_ADDR_t                 INIT;               /* offset 0x0118 */
	// ADMA_STATUS_t                   STAT;               /* offset 0x011C */
	// ADMA_NEXT_ADDR_t                NEXT;               /* offset 0x0120 */
} *AU_DFE_ADMA_PTR_t;




#endif
