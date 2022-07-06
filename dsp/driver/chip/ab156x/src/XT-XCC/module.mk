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

ifeq ($(AIR_AUDIO_HARDWARE_ENABLE),y)
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_afe_connection.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_afe_driver.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_afe_clock.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_afe_control.c
endif
C_SRC  +=  $(AB156X_CHIP)/src/hal_uart.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_nvic.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_nvic_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_hw_semaphore.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_gpt_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_gpt.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_cache.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_cache_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_core_status.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_pdma_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_log.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_eint.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_eint_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_spi_master.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_spi_master_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_sw_dma.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_sw_dma_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_ccni.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_ccni_config.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_ccni_internal.c
#C_SRC  +=  $(AB156X_CHIP)/src/hal_usb.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_gpio.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_gpio_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_dvfs.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_clock.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_dwt.c
ifeq ($(AIR_AUDIO_HARDWARE_ENABLE),y)
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_control.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_path.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_clock.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_driver.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_audio_volume.c
endif
C_SRC  +=  $(AB156X_CHIP)/src/hal_i2c_master.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_i2c_master_internal.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_time_check.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_trng.c
C_SRC  +=  $(AB156X_CHIP)/src/hal_trng_internal.c
###################################################
#include path

INC  +=  $(AB156X_CHIP)/inc
INC  +=  driver/chip/inc
INC  +=  driver/board/$(BOARD_CONFIG)/hw_resource_assignment/inc
