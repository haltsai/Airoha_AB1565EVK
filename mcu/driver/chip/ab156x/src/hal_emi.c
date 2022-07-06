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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_platform.h"
#include <memory_attribute.h>
#include <hal_emi_internal.h>

//#define EMI_SUPPORT_DEEP_SLEEP_BACKUP  // SPM will help to do EMI enter & exit sleep and backup & restore
//#define EMI_NEED_DO_SLEEP_BY_SW

//#define EMI_FPGA_SETTING   
/*#define EMI_FPGA_SETTING 
1. disable exit half sleep feature(TDI FPGA have no 32K clock)
2. disable enable 1/5T DLL cali feature,only set a max DLL vaule(FPGA only 26M clock,so no need DLL cali)
3. set max driving(Tx and Rx) value 
*/

#ifdef EMI_FPGA_SETTING
ATTR_RWDATA_IN_CACHED_SYSRAM EMI_SETTINGS emi_settings[] = {
    {
        0x00000000,     /*EMI_GENA_VAL*/
        0x00000000,     /*EMI_RDCT_VAL*/
        0x00000003,     /*EMI_DSRAM_VAL,should double confirm with DE when FPGA and real chip sample back*/
        0x00020A06,     /*EMI_MSRAM0_VAL*/
        0x00010000,     /*EMI_MSRAM1_VAL*/
        0x00000000,     /*EMI_IDL_C_VAL*/
        0x00000000,     /*EMI_IDL_D_VAL*/
        0x00000000,     /*EMI_IDL_E_VAL*/
        0x00000000,     /*EMI_ODL_C_VAL*/
        0x00000000,     /*EMI_ODL_D_VAL*/
        0x00000000,     /*EMI_ODL_E_VAL*/
        0x00000000,     /*EMI_ODL_F_VAL*/
        0x00040004,     /*EMI_IO_A_VAL*/
        0x00040004,     /*EMI_IO_B_VAL*/
    } ,
};
#else
ATTR_RWDATA_IN_CACHED_SYSRAM EMI_SETTINGS emi_settings[] = {
    {
        0x00000100,     /*EMI_GENA_VAL*/
        0x00000000,     /*EMI_RDCT_VAL*/
        0x00000004,     /*EMI_DSRAM_VAL,should double confirm with DE when FPGA and real chip sample back*/
        0x00020A06,     /*EMI_MSRAM0_VAL*/
        0x00010000,     /*EMI_MSRAM1_VAL*/
        0x00000000,     /*EMI_IDL_C_VAL*/
        0x00000000,     /*EMI_IDL_D_VAL*/
        0x00000000,     /*EMI_IDL_E_VAL*/
        0x00000000,     /*EMI_ODL_C_VAL*/
        0x00000000,     /*EMI_ODL_D_VAL*/
        0x00000000,     /*EMI_ODL_E_VAL*/
        0x00000000,     /*EMI_ODL_F_VAL*/
        0x00000000,     /*EMI_IO_A_VAL*/
        0x00000000,     /*EMI_IO_B_VAL*/
    } ,
};

#endif

#define PSRAM_BASE 0x0

ATTR_RWDATA_IN_CACHED_SYSRAM uint32_t __EMI_CurSR0 = 0x0;
ATTR_RWDATA_IN_CACHED_SYSRAM uint32_t __EMI_CurSR1 = 0x0;
ATTR_RWDATA_IN_CACHED_SYSRAM uint32_t __EMI_CurSR2 = 0x0;

#define CM4_BOOT_FROM_SLV    ((volatile uint32_t *)(0xA2280008))

ATTR_RWDATA_IN_CACHED_SYSRAM EMI_REGISTER_T *emi_register = (EMI_REGISTER_T *)EMI_BASE;
ATTR_RWDATA_IN_CACHED_SYSRAM EMI_AO_REGISTER_T *emi_ao_register = (EMI_AO_REGISTER_T *)EMI_AO_BASE;

#ifdef EMI_NEED_DO_SLEEP_BY_SW
#ifdef EMI_SUPPORT_DEEP_SLEEP_BACKUP
ATTR_ZIDATA_IN_TCM EMI_SETTINGS emi_register_backup;
ATTR_ZIDATA_IN_TCM uint32_t EMI_CONM_VAL_backup;
#endif
#endif

ATTR_TEXT_IN_SYSRAM uint32_t __EMI_GetSR(uint32_t bank_no)
{
    uint32_t value;

    emi_register->EMI_MREG_RW = bank_no << EMI_MREG_BANK_OFFSET ; //set bank
    emi_register->EMI_MREG_RW = EMI_MRGE_EDGE_TRIGGER_MASK | (emi_register->EMI_MREG_RW);  //trigger start

    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0); //wait read data ready

    value = ((emi_register->EMI_MREG_RW & EMI_MREG_RDATA_MASK) >> EMI_MREG_RDATA_OFFSET);

    return value;
}


ATTR_TEXT_IN_SYSRAM void __EMI_SetSR(uint32_t bank_no, uint32_t value)
{

    value = value & EMI_MR_DATA_MASK;
    emi_register->EMI_MREG_RW = (value << EMI_MRGE_WDATA_OFFSET) | (bank_no << EMI_MREG_BANK_OFFSET) | (1 << EMI_MRGE_W_OFFSET) ; //set write data¡Bbank and write access
    emi_register->EMI_MREG_RW = EMI_MRGE_EDGE_TRIGGER_MASK | (emi_register->EMI_MREG_RW);  //trigger start

    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0); //wait write data ready

    return ;
}

ATTR_TEXT_IN_SYSRAM void EMI_PowerOn_Init(void)
{

    /* boot slave default Enable and Make EMI_base + 0 and  EMI_base + 4 mapping to
    boot vector. we need to disable it to make EMI_base map to real PSRAM*/
    *(CM4_BOOT_FROM_SLV) = 0x0;

#ifndef EMI_FPGA_SETTING
    emi_register->EMI_HFSLP_UNION.EMI_HFSLP_CELLS.HFSLP_EXIT_REQ = 0x1;
    while(emi_register->EMI_HFSLP_UNION.EMI_HFSLP_CELLS.HFSLP_EXIT_ACK ==0);
    emi_register->EMI_HFSLP_UNION.EMI_HFSLP = 0x0;
     while(emi_register->EMI_HFSLP_UNION.EMI_HFSLP_CELLS.HFSLP_EXIT_ACK ==1);
#endif

    //*((volatile uint32_t *)(PSRAM_BASE)) = 0x5a5a5a5a;
    //*((volatile uint32_t *)(PSRAM_BASE + 0x4)) = 0x5a5a5a5a;

    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0);

    #ifdef EMI_FPGA_SETTING
    __EMI_SetSR(0x0,0x9);/*latency = 2  and half drive strengh*/
    #else
    __EMI_SetSR(0x0,0xf);//latency = 3, driving = 1/8
    //this function called by BL, because BL run @78M so,PSRAM setting latency = 3, driving = 1/8
    #endif

   __EMI_SetSR(0x4,0x8);
    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0);





}
ATTR_TEXT_IN_SYSRAM int8_t hal_emi_configure(void)
{
    return custom_setEMI();
}

ATTR_TEXT_IN_SYSRAM int8_t custom_setEMI(void)
{
    #ifdef MTK_NO_PSRAM_ENABLE
        return 0 ;
    #endif


    EMI_PowerOn_Init();


    __EMI_CurSR0 = __EMI_GetSR(0);
   __EMI_CurSR1 = __EMI_GetSR(1);
   __EMI_CurSR2 = __EMI_GetSR(2);

    return 0;
}

//#define EMI_ARB_A_VAL                           0x00005431   // ARM  port, filter-length=1024 (max=4096)
//#define EMI_ARB_B_VAL                           0x00005009   // DMA port
//#define EMI_ARB_C_VAL                           0x00005046   // MM

#define EMI_ARB_M0_BW                           0x31   // ARM  port, filter-length=1024 (max=4096)
#define EMI_ARB_M1_BW                           0x09   // DMA port
#define EMI_ARB_M2_BW                           0x46   // MM


ATTR_TEXT_IN_SYSRAM int __EMI_EnableBandwidthLimiter( void )
{

    emi_register->EMI_ARBA0_UNION.EMI_ARBA0_CELLS.BW_FILTER_LEN = 0x1;/*1024 cycles*/
    /* Set ARM port BW*/
    emi_register->EMI_ARBA0_UNION.EMI_ARBA0_CELLS.SOFT_MODE_SETTING = 0x1;/*soft mode*/
    emi_register->EMI_ARBA0_UNION.EMI_ARBA0_CELLS.MAX_GNT_CNT = EMI_ARB_M0_BW;
    emi_register->EMI_ARBA0_UNION.EMI_ARBA0_CELLS.BW_FILTER_EN = 0x1;/*enable BW*/

    /* Set DMA port BW */
    emi_register->EMI_ARBB0_UNION.EMI_ARBB0_CELLS.SOFT_MODE_SETTING = 0x1;/*soft mode*/
    emi_register->EMI_ARBB0_UNION.EMI_ARBB0_CELLS.MAX_GNT_CNT = EMI_ARB_M1_BW;
    emi_register->EMI_ARBB0_UNION.EMI_ARBB0_CELLS.BW_FILTER_EN = 0x1;/*enable BW*/

    /* Set GMC port BW */
    emi_register->EMI_ARBC0_UNION.EMI_ARBC0_CELLS.SOFT_MODE_SETTING = 0x1;/*soft mode*/
    emi_register->EMI_ARBC0_UNION.EMI_ARBC0_CELLS.MAX_GNT_CNT = EMI_ARB_M2_BW;
    emi_register->EMI_ARBC0_UNION.EMI_ARBC0_CELLS.BW_FILTER_EN = 0x1;/*enable BW*/

    return 0;

}

ATTR_TEXT_IN_SYSRAM int32_t TestCase_MBIST(void)
{
    uint32_t mbist_src, mbist_len, mbist_data[2] = {0x5AA5, 0xFFFF};
    uint32_t i, bist_data_inv,  bist_data_random;

    mbist_src = PSRAM_BASE+0x300000;
    mbist_len = 0x4000; //16k Bytes

    emi_register->EMI_MBISTA = 0x0;
    for (bist_data_inv = 0; bist_data_inv <= 1; bist_data_inv++) {
        for (i = 0; i <= 1; i++) {
            for (bist_data_random = 0; bist_data_random <= 1; bist_data_random++) {
                // need to reset mbist everytime
                emi_register->EMI_MBISTA = 0x0;
                // config mbist source address and test length
                emi_register->EMI_MBISTB = ((mbist_src >> 8) << EMI_BIST_STR_ADDR_OFFSET) | (( mbist_len >> 8) - 1);
                // config mbist data pattern, data inverse, burst length and width
                emi_register->EMI_MBISTA = (mbist_data[i] << EMI_BIST_BG_DATA_OFFSET) | (bist_data_inv << EMI_BIST_DATA_INV_OFFSET)  | (bist_data_random << EMI_BIST_DATA_RANDOM_OFFSET );
                emi_register->EMI_MBISTA |= 0x325; // enable MBIST, write and read mode, 4 byte trans size, burst length = 16 byte
                // check if mbist finished
                while ((emi_register->EMI_MBISTD & EMI_BIST_END_MASK) == 0x0);
                // check mbist result
                if ((emi_register->EMI_MBISTD & EMI_BIST_FAIL_MASK) != 0) {
                    emi_register->EMI_MBISTA = 0x0;
                    return -1;
                }
            }
        }
    }

    emi_register->EMI_MBISTA = 0x0;
    return 0;
}
ATTR_TEXT_IN_SYSRAM int32_t __EMI_DataAutoTrackingMbistTest(void)
{
    if ( TestCase_MBIST() != 0 ) {
        return -1;
    }

    return 0;
}

#define DATA_TUNING_STEP 1

ATTR_TEXT_IN_SYSRAM void __EMI_EnableDataAutoTracking(void)
{
    int32_t dqy_in_del = 0x1F;
    int32_t dqs_in_del = 0;

    emi_ao_register->EMI_IDLC_UNION.EMI_IDLC = 0X0;
    emi_ao_register->EMI_IDLD_UNION.EMI_IDLD = 0X0;
    emi_ao_register->EMI_IDLE_UNION.EMI_IDLE = 0X0;

    for (dqy_in_del = 0x1F; dqy_in_del >= 0; dqy_in_del -= DATA_TUNING_STEP) {
        emi_ao_register->EMI_IDLC_UNION.EMI_IDLC = dqy_in_del << EMI_DQ7_IN_DEL_OFFSET | dqy_in_del << EMI_DQ6_IN_DEL_OFFSET | dqy_in_del << EMI_DQ5_IN_DEL_OFFSET | dqy_in_del;
        emi_ao_register->EMI_IDLD_UNION.EMI_IDLD = dqy_in_del << EMI_DQ3_IN_DEL_OFFSET | dqy_in_del << EMI_DQ2_IN_DEL_OFFSET | dqy_in_del << EMI_DQ1_IN_DEL_OFFSET | dqy_in_del;

        if (0 ==  __EMI_DataAutoTrackingMbistTest()) {
            break;
        }
    }

    if (dqy_in_del < 0) { //DQ_IN_DELAY can't find window
        for (dqs_in_del = 0x1; dqs_in_del <= 0x1F; dqs_in_del += DATA_TUNING_STEP) {
            emi_ao_register->EMI_IDLE_UNION.EMI_IDLE = dqs_in_del << EMI_DQS0_IN_DEL_OFFSET ;

            if (0 ==  __EMI_DataAutoTrackingMbistTest()) {
                break;
            }
        }
    }
    return;
}

ATTR_TEXT_IN_SYSRAM int8_t hal_emi_configure_advanced(void)
{
    return custom_setAdvEMI();
}


ATTR_TEXT_IN_SYSRAM int8_t custom_setAdvEMI(void)
{
    #ifdef MTK_NO_PSRAM_ENABLE
        return 0;
    #endif


    /**
        * Switch the EMI register into corresponding modes.
        */

    emi_ao_register->EMI_IOA = emi_settings[0].EMI_IO_A_VAL;
    emi_ao_register->EMI_IOB = emi_settings[0].EMI_IO_B_VAL;

    
    emi_register->EMI_GENA = emi_settings[0].EMI_GENA_VAL;
    emi_register->EMI_RDCT = emi_settings[0].EMI_RDCT_VAL;

    emi_register->EMI_DSRAM = emi_settings[0].EMI_DSRAM_VAL;
    emi_register->EMI_MSRAM0_UNION.EMI_MSRAM0 = emi_settings[0].EMI_MSRAM0_VAL;
    emi_register->EMI_MSRAM1_UNION.EMI_MSRAM1 = emi_settings[0].EMI_MSRAM1_VAL;

    /**
      * set driving & delay
      */
    emi_ao_register->EMI_IDLC_UNION.EMI_IDLC = emi_settings[0].EMI_IDL_C_VAL;
    emi_ao_register->EMI_IDLD_UNION.EMI_IDLD = emi_settings[0].EMI_IDL_D_VAL;
    emi_ao_register->EMI_IDLE_UNION.EMI_IDLE = emi_settings[0].EMI_IDL_E_VAL;

    emi_ao_register->EMI_ODLC_UNION.EMI_ODLC = emi_settings[0].EMI_ODL_C_VAL;
    emi_ao_register->EMI_ODLD_UNION.EMI_ODLD = emi_settings[0].EMI_ODL_D_VAL;
    emi_ao_register->EMI_ODLE_UNION.EMI_ODLE = emi_settings[0].EMI_ODL_E_VAL;
    emi_ao_register->EMI_ODLF_UNION.EMI_ODLF = emi_settings[0].EMI_ODL_F_VAL;

emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x0;	 /*1.	*A0050030=0x0 (EMI_DLLV0)*/


#ifdef EMI_FPGA_SETTING
    /*DLL init flow for FPGA--because of 26M too slow,so no need to enable DataAutoTracking&enable 1/5 T,only set the DQS as max value*/
    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x0;	 /*1.	*A0050030=0x0 (EMI_DLLV0)*/
    emi_ao_register->EMI_IDLE_UNION.EMI_IDLE = 0x1e0000;/*2.	*A0050048=0x1e0000(EMI_IDLE)*/
#else
   //__EMI_EnableDataAutoTracking();//DE confirm no need data auto cali from MT2625
    /*Enable 1/5 DLL*/

    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0003;//dll_cal_init = 1  cal_en = 1
    while(emi_register->EMI_DLLV1_UNION.EMI_DLLV1_CELLS.CAL_DONE != 0x1);
    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0007;  // enable dll soft update
    while(emi_register->EMI_DLLV1_UNION.EMI_DLLV1_CELLS.CAL_DONE != 0x0); //wait_cal_done = 0
    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0001; //initialization finish

#endif

    __EMI_EnableBandwidthLimiter();

    return 0;
}

#ifdef EMI_NEED_DO_SLEEP_BY_SW
#ifdef EMI_SUPPORT_DEEP_SLEEP_BACKUP

ATTR_TEXT_IN_SYSRAM void EMI_Setting_Save(void)
{ 
    EMI_CONM_VAL_backup                  = emi_register->EMI_CONM_UNION.EMI_CONM;
    
    emi_register_backup.EMI_GENA_VAL     = emi_register->EMI_GENA;
    emi_register_backup.EMI_RDCT_VAL     = emi_register->EMI_RDCT;
    emi_register_backup.EMI_DSRAM_VAL    = emi_register->EMI_DSRAM;
    emi_register_backup.EMI_MSRAM0_VAL   = emi_register->EMI_MSRAM0_UNION.EMI_MSRAM0;
    emi_register_backup.EMI_MSRAM1_VAL   = emi_register->EMI_MSRAM1_UNION.EMI_MSRAM1;
    emi_register_backup.EMI_IDL_C_VAL    = emi_ao_register->EMI_IDLC_UNION.EMI_IDLC;
    emi_register_backup.EMI_IDL_D_VAL    = emi_ao_register->EMI_IDLD_UNION.EMI_IDLD;
    emi_register_backup.EMI_IDL_E_VAL    = emi_ao_register->EMI_IDLE_UNION.EMI_IDLE;
    emi_register_backup.EMI_ODL_C_VAL    = emi_register->EMI_ODLC_UNION.EMI_ODLC;
    emi_register_backup.EMI_ODL_D_VAL    = emi_register->EMI_ODLD_UNION.EMI_ODLD;
    emi_register_backup.EMI_ODL_E_VAL    = emi_register->EMI_ODLE_UNION.EMI_ODLE;
    emi_register_backup.EMI_ODL_F_VAL    = emi_register->EMI_ODLF_UNION.EMI_ODLF;
    emi_register_backup.EMI_IO_B_VAL     = emi_ao_register->EMI_IOB;
    emi_register_backup.EMI_IO_A_VAL     = emi_ao_register->EMI_IOA;
}

ATTR_TEXT_IN_SYSRAM void EMI_Setting_restore(void)
{
    while((emi_register->EMI_CONM_UNION.EMI_CONM_CELLS.EMI_SRAM_IDLE == 0) || (emi_register->EMI_CONM_UNION.EMI_CONM_CELLS.EMI_IDLE == 0));
    
    emi_register->EMI_GENA                    =   emi_register_backup.EMI_GENA_VAL;
    emi_register->EMI_RDCT                    =   emi_register_backup.EMI_RDCT_VAL;
    emi_register->EMI_DSRAM                   =   emi_register_backup.EMI_DSRAM_VAL;
    emi_register->EMI_MSRAM0_UNION.EMI_MSRAM0 =   emi_register_backup.EMI_MSRAM0_VAL;
    emi_register->EMI_MSRAM1_UNION.EMI_MSRAM1 =   emi_register_backup.EMI_MSRAM1_VAL;
    emi_ao_register->EMI_IDLC_UNION.EMI_IDLC     =   emi_register_backup.EMI_IDL_C_VAL;
    emi_ao_register->EMI_IDLD_UNION.EMI_IDLD     =   emi_register_backup.EMI_IDL_D_VAL;
    emi_ao_register->EMI_IDLE_UNION.EMI_IDLE     =   emi_register_backup.EMI_IDL_E_VAL;
    emi_register->EMI_ODLC_UNION.EMI_ODLC     =   emi_register_backup.EMI_ODL_C_VAL;
    emi_register->EMI_ODLD_UNION.EMI_ODLD     =   emi_register_backup.EMI_ODL_D_VAL;
    emi_register->EMI_ODLE_UNION.EMI_ODLE     =   emi_register_backup.EMI_ODL_E_VAL;
    emi_register->EMI_ODLF_UNION.EMI_ODLF     =   emi_register_backup.EMI_ODL_F_VAL;
    emi_ao_register->EMI_IOB                  =   emi_register_backup.EMI_IO_B_VAL;
    emi_ao_register->EMI_IOA                  =   emi_register_backup.EMI_IO_A_VAL;


    __EMI_EnableBandwidthLimiter();
}
#endif
#endif

#ifdef EMI_NEED_DO_SLEEP_BY_SW
ATTR_TEXT_IN_SYSRAM void mtk_psram_half_sleep_exit(void)
{
    #ifdef MTK_NO_PSRAM_ENABLE
        return ;
    #endif

    emi_ao_register->EMI_IOB = emi_ao_register->EMI_IOB & 0xFFDFFFDF;
    /* TINFO=" ----- exit half sleep mode begin ----- " */
    //exit half sleep
    //emi_register->EMI_HFSLP = 0x1;
    //while((emi_register->EMI_HFSLP & EMI_HFSLP_EXIT_ACK_MASK) >> EMI_HFSLP_EXIT_ACK_OFFSET == 0);
    //emi_register->EMI_HFSLP = 0x0;
    //while((emi_register->EMI_HFSLP & EMI_HFSLP_EXIT_ACK_MASK) >> EMI_HFSLP_EXIT_ACK_OFFSET == 1);

    emi_register->EMI_HFSLP_UNION.EMI_HFSLP_CELLS.HFSLP_EXIT_REQ = 0x1;
    while(emi_register->EMI_HFSLP_UNION.EMI_HFSLP_CELLS.HFSLP_EXIT_ACK ==0);
    emi_register->EMI_HFSLP_UNION.EMI_HFSLP = 0x0;
    while(emi_register->EMI_HFSLP_UNION.EMI_HFSLP_CELLS.HFSLP_EXIT_ACK ==1);

#ifdef EMI_SUPPORT_DEEP_SLEEP_BACKUP
    EMI_Setting_restore();
#endif

    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x0;    /*1.*A0050030=0x0 (EMI_DLLV0)*/
    /*Enable 1/5 DLL*/
    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0003;//dll_cal_init = 1  cal_en = 1
    while(emi_register->EMI_DLLV1_UNION.EMI_DLLV1_CELLS.CAL_DONE != 0x1);
    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0007;  // enable dll soft update
    while(emi_register->EMI_DLLV1_UNION.EMI_DLLV1_CELLS.CAL_DONE != 0x0); //wait_cal_done = 0
    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0001; //initialization finish

#ifdef EMI_SUPPORT_DEEP_SLEEP_BACKUP
    emi_register->EMI_CONM_UNION.EMI_CONM = EMI_CONM_VAL_backup;
#endif
    emi_register->EMI_CONM_UNION.EMI_CONM_CELLS.REQ_MASK = 0x0;//unmask EMI
    /* TINFO=" ----- exit half sleep mode end ----- " */
}

ATTR_TEXT_IN_SYSRAM void mtk_psram_half_sleep_enter(void)
{
    #ifdef MTK_NO_PSRAM_ENABLE
        return ;
    #endif

    #ifdef EMI_SUPPORT_DEEP_SLEEP_BACKUP
        EMI_Setting_Save();
    #endif
    
    /* TINFO= " ----- enter half sleep mode begin ----- " */
    /*enter half sleep*/
    while((emi_register->EMI_CONM_UNION.EMI_CONM_CELLS.EMI_SRAM_IDLE == 0) || (emi_register->EMI_CONM_UNION.EMI_CONM_CELLS.EMI_IDLE == 0));
    emi_register->EMI_CONM_UNION.EMI_CONM_CELLS.REQ_MASK =  EMI_REQ_MASK_MASK;
    emi_register->EMI_MREG_RW = 0xf0670000;
    while((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) ==0);
    emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x00000000;
    while(emi_register->EMI_HFSLP_UNION.EMI_HFSLP_CELLS.HFSLP_ENT_STA==0);
  
    /* TINFO= " ----- enter half sleep mode end  ----- " */
    emi_ao_register->EMI_IOB = emi_ao_register->EMI_IOB | 0x200020;
}
#endif

ATTR_TEXT_IN_SYSRAM int32_t EMI_DynamicClockSwitch(emi_clock clock)
{
        volatile uint32_t delay;
#ifdef MTK_NO_PSRAM_ENABLE
        return -1;	
#endif


       if(clock == EMI_CLK_LOW_TO_HIGH)
       {
            emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x0;    /*1.*A0050030=0x0 (EMI_DLLV0)*/
            /*Enable 1/5 DLL*/
            emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0003;//dll_cal_init = 1  cal_en = 1
            while(emi_register->EMI_DLLV1_UNION.EMI_DLLV1_CELLS.CAL_DONE != 0x1);
            emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0007;  // enable dll soft update
            while(emi_register->EMI_DLLV1_UNION.EMI_DLLV1_CELLS.CAL_DONE != 0x0); //wait_cal_done = 0
            emi_register->EMI_DLLV0_UNION.EMI_DLLV0 = 0x000f0001; //initialization finish
    
            for(delay = 0; delay<0xFF; delay++);

  //          emi_register->EMI_CONM = emi_register->EMI_CONM & (~0x700);   //unmask request
       }
       else if(clock == EMI_CLK_HIGH_TO_LOW)
            return 0;
       else
            return -1;

       return 0;
}

ATTR_TEXT_IN_SYSRAM int32_t hal_emi_dynamic_clock_switch(emi_clock clock,hal_emi_clock_frq target_freq)
{
#ifdef MTK_NO_PSRAM_ENABLE
        return -1;	
#endif
    if(target_freq == EMI_CLK_26M)
    {
       emi_register->EMI_GENA = 1<<24; //when run @26M with 0.9v, emi need a special setting
       emi_ao_register->EMI_IOA = 0x0f000f00;//when run @26M with 0.9v, emi need a special setting
       emi_ao_register->EMI_IOB = 0x0f000f00;//when run @26M with 0.9v, emi need a special setting
       emi_register->EMI_DSRAM = 0x3;

       __EMI_SetSR(0x0,0xb);//latency = 2, driving = 1/8
    }
    else if(target_freq == EMI_CLK_78M)
    {
       emi_register->EMI_GENA = 0;
       emi_ao_register->EMI_IOA = emi_settings[0].EMI_IO_A_VAL;;
       emi_ao_register->EMI_IOB = emi_settings[0].EMI_IO_B_VAL;;
       emi_register->EMI_DSRAM = emi_settings[0].EMI_DSRAM_VAL;
       __EMI_SetSR(0x0,0xf);//latency = 3, driving = 1/8
    }
    else if(target_freq == EMI_CLK_104M)
    {
       emi_register->EMI_GENA = 0;
       emi_ao_register->EMI_IOA = emi_settings[0].EMI_IO_A_VAL;;
       emi_ao_register->EMI_IOB = emi_settings[0].EMI_IO_B_VAL;;
       emi_register->EMI_DSRAM = emi_settings[0].EMI_DSRAM_VAL;
       __EMI_SetSR(0x0,0xf);//latency = 3, driving = 1/8
    }
    else
    {
    }
    EMI_DynamicClockSwitch(clock);
       
       return 0;
}




ATTR_RWDATA_IN_CACHED_SYSRAM volatile uint32_t dvfs_dma_runing_status = 0x0;

ATTR_TEXT_IN_SYSRAM void emi_mask_master(void)
{
#ifdef MTK_NO_PSRAM_ENABLE
        return ;
#endif
  emi_register->EMI_CONM_UNION.EMI_CONM_CELLS.REQ_MASK = EMI_REQ_MASK_MASK;   //mask request
}

ATTR_TEXT_IN_SYSRAM void emi_unmask_master(void)
{
#ifdef MTK_NO_PSRAM_ENABLE
        return ;
#endif
  emi_register->EMI_CONM_UNION.EMI_CONM_CELLS.REQ_MASK = 0x0;   //unmask request
}


