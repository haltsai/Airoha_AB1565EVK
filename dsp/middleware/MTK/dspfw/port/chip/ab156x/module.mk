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


###################################################
DSPFW_PATH = middleware/MTK/dspfw/port/chip/$(IC_CONFIG)

C_SRC += $(DSPFW_PATH)/src/dsp.c
C_SRC += $(DSPFW_PATH)/src/dsp_report_entry_list.c
C_SRC += $(DSPFW_PATH)/src/dsp_scenario.c
C_SRC += $(DSPFW_PATH)/src/dsp_sdk.c
C_SRC += $(DSPFW_PATH)/src/dsp_temp.c
C_SRC += $(DSPFW_PATH)/src/dsp_update_entry_list.c
C_SRC += $(DSPFW_PATH)/src/dsp_audio_msg.c

ifeq ($(AIR_AUDIO_HARDWARE_ENABLE),y)
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_drv/dsp_drv_afe.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_drv/dsp_drv_dfe.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_drv/audio_afe_common.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_drv/audio_afe_pcm_dl1.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_drv/audio_afe_pcm_ul1.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_drv/audio_afe_pcm_awb.c
ifeq ($(AIR_I2S_SLAVE_ENABLE),y)
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_drv/audio_i2s_slave.c
endif
endif

C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_drv/bt_interface.c

C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_audio_ctrl.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_command_dispatcher.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_memory.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_buffer.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_callback.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_stream_connect.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_audio_process.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_feature_interface.c
ifeq ($(AIR_AUDIO_HARDWARE_ENABLE),y)
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_gain_control.c
endif
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_update_para.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_report.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_nvkey.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_dump.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_vad.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_interface/dsp_vow.c

C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_stream/stream_audio_driver.c
ifeq ($(AIR_AUDIO_HARDWARE_ENABLE),y)
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_stream/stream_audio_hardware.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_stream/stream_audio_setting.c
endif
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dsp_stream/silence_detection_interface.c

C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dtm/dtm.c
C_SRC += $(DSPFW_PATH)/src/dsp_lower_layer/dtm/dtm_audio_verification.c

C_SRC += $(DSPFW_PATH)/src/dsp_upper_layer/davt/davt.c
C_SRC += $(DSPFW_PATH)/src/dsp_upper_layer/dhpt/dhpt.c
C_SRC += $(DSPFW_PATH)/src/dsp_upper_layer/dprt/dprt.c
C_SRC += $(DSPFW_PATH)/src/dsp_upper_layer/dllt/dllt.c
C_SRC += $(DSPFW_PATH)/src/dsp_upper_layer/dsp_task/dsp_task.c

###################################################
# include path
INC += middleware/MTK/stream/inc

INC += $(DSPFW_PATH)/inc
INC += $(DSPFW_PATH)/inc/dsp
INC += $(DSPFW_PATH)/inc/voice_plc
INC += $(DSPFW_PATH)/inc/system

INC += $(DSPFW_PATH)/inc/dsp_lower_layer/dsp_drv
INC += $(DSPFW_PATH)/inc/dsp_lower_layer/dsp_interface
INC += $(DSPFW_PATH)/inc/dsp_lower_layer/dsp_interface/nvdm
INC += $(DSPFW_PATH)/inc/dsp_lower_layer/dsp_stream
INC += $(DSPFW_PATH)/inc/dsp_lower_layer/dtm
INC += $(DSPFW_PATH)/inc/dsp_upper_layer

INC  +=  driver/chip/inc
INC  +=  driver/chip/$(IC_CONFIG)/inc
