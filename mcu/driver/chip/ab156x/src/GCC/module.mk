# Copyright Statement:                                                                                               
#                                                                                                                    
# (C) 2017  Airoha Technology Corp. All rights reserved.                                                             
#                                                                                                                    
# This software/firmware and related documentation ("Airoha Software") are                                           
# protected under relevant copyright laws. The information contained herein                                          
# is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.                        
# Without the prior written permission of Airoha and/or its licensors,                                               
# any reproduction, modification, use or disclosure of Airoha Software,                                              
# and information contained herein, in whole or in part, shall be strictly prohibited.                               
# You may only use, reproduce, modify, or distribute (as applicable) Airoha Software                                 
# if you have agreed to and been bound by the applicable license agreement with                                      
# Airoha ("License Agreement") and been granted explicit permission to do so within                                  
# the License Agreement ("Permitted User").  If you are not a Permitted User,                                        
# please cease any access or use of Airoha Software immediately.                                                     
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES                                        
# THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES                                               
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL                          
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF                             
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.                                              
# NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE                                            
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR                                              
# SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH                                            
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES                               
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES                       
# CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA                                  
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR                                   
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND                               
# CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,                                   
# AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,                                                 
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO                                          
# AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.                                                                          
#                                                                                                                    


AB156X_CHIP = driver/chip/ab156x
COMPONENT = driver/board/component


CFLAGS   += $(FPUFLAGS) -DUSE_HAL_DRIVER

C_FILES  += $(AB156X_CHIP)/src/hal_emi.c
C_FILES  += $(AB156X_CHIP)/src/hal_uart.c
C_FILES  += $(AB156X_CHIP)/src/hal_uart_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_log.c
C_FILES  += $(AB156X_CHIP)/src/hal_nvic.c
C_FILES  += $(AB156X_CHIP)/src/hal_nvic_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_eint.c
C_FILES  += $(AB156X_CHIP)/src/hal_eint_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_pdma_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_gpt.c
C_FILES  += $(AB156X_CHIP)/src/hal_gpt_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_hw_semaphore.c
C_FILES  += $(AB156X_CHIP)/src/hal_clock.c
C_FILES  += $(AB156X_CHIP)/src/hal_dcxo.c
C_FILES  += $(AB156X_CHIP)/src/hal_gpio.c
C_FILES  += $(AB156X_CHIP)/src/hal_gpio_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_dwt.c
C_FILES  += $(AB156X_CHIP)/src/hal_msdc.c
C_FILES  += $(AB156X_CHIP)/src/hal_sd.c
C_FILES  += $(AB156X_CHIP)/src/hal_sd_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_sdio.c
C_FILES  += $(AB156X_CHIP)/src/hal_sdio_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_flash_combo_init.c
C_FILES  += $(AB156X_CHIP)/src/hal_flash_combo_nor.c
C_FILES  += $(AB156X_CHIP)/src/hal_flash_custom.c
C_FILES  += $(AB156X_CHIP)/src/hal_flash_disk.c
C_FILES  += $(AB156X_CHIP)/src/hal_flash_mtd.c
C_FILES  += $(AB156X_CHIP)/src/hal_flash_mtd_common.c
C_FILES  += $(AB156X_CHIP)/src/hal_flash_custom_sfi.c
C_FILES  += $(AB156X_CHIP)/src/hal_flash_mtd_dal.c
C_FILES  += $(AB156X_CHIP)/src/hal_wdt.c
C_FILES  += $(AB156X_CHIP)/src/hal_wdt_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_ccni.c
C_FILES  += $(AB156X_CHIP)/src/hal_ccni_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_ccni_config.c
C_FILES  += $(AB156X_CHIP)/src/hal_cache.c
C_FILES  += $(AB156X_CHIP)/src/hal_cache_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_mpu.c
C_FILES  += $(AB156X_CHIP)/src/hal_mpu_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_core_status.c
C_FILES  += $(AB156X_CHIP)/src/hal_spi_master.c
C_FILES  += $(AB156X_CHIP)/src/hal_spi_master_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_spi_slave.c
C_FILES  += $(AB156X_CHIP)/src/hal_spi_slave_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_i2c_master.c
C_FILES  += $(AB156X_CHIP)/src/hal_i2c_master_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_pmu.c
ifeq ($(IC_TYPE),ab1565)
C_FILES  += $(AB156X_CHIP)/src/hal_pmu_internal_2565.c
C_FILES  += $(AB156X_CHIP)/src/hal_pmu_cali_2565.c
C_FILES  += $(AB156X_CHIP)/src/hal_pmu_charger_2565.c
C_FILES  += $(AB156X_CHIP)/src/hal_pmu_auxadc_2565.c
else ifeq ($(IC_TYPE),ab1568)
C_FILES  += $(AB156X_CHIP)/src/hal_pmu_internal_2568.c
C_FILES  += $(AB156X_CHIP)/src/hal_pmu_charger_2568.c
C_FILES  += $(AB156X_CHIP)/src/hal_pmu_auxadc_2568.c
endif
C_FILES  += $(AB156X_CHIP)/src/hal_dvfs.c
C_FILES  += $(AB156X_CHIP)/src/hal_dvfs_internal_vcore.c
C_FILES  += $(AB156X_CHIP)/src/hal_adc.c
C_FILES  += $(AB156X_CHIP)/src/hal_usb.c
C_FILES  += $(AB156X_CHIP)/src/hal_usb_phy.c
C_FILES  += $(AB156X_CHIP)/src/hal_trng.c
C_FILES  += $(AB156X_CHIP)/src/hal_trng_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_audio.c
C_FILES  += $(AB156X_CHIP)/src/hal_audio_dsp_controller.c
C_FILES  += $(AB156X_CHIP)/src/hal_aes.c
C_FILES  += $(AB156X_CHIP)/src/hal_aesotf.c
C_FILES  += $(AB156X_CHIP)/src/hal_aesotf_esc.c
C_FILES  += $(AB156X_CHIP)/src/hal_sha.c
C_FILES  += $(AB156X_CHIP)/src/hal_sw_dma.c
C_FILES  += $(AB156X_CHIP)/src/hal_sw_dma_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_captouch.c
C_FILES  += $(AB156X_CHIP)/src/hal_captouch_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_rtc.c
C_FILES  += $(AB156X_CHIP)/src/hal_rtc_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_irrx.c
C_FILES  += $(AB156X_CHIP)/src/hal_irrx_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_pwm.c
C_FILES  += $(AB156X_CHIP)/src/hal_isink.c
C_FILES  += $(AB156X_CHIP)/src/hal_esc.c
C_FILES  += $(AB156X_CHIP)/src/hal_esc_internal.c
C_FILES  += $(AB156X_CHIP)/src/hal_time_check.c
C_FILES  += $(AB156X_CHIP)/src/hal_security.c
#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/ab156x/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc

CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/ab156x/Include
CFLAGS  += -I$(SOURCE_DIR)/driver/CMSIS/Include
