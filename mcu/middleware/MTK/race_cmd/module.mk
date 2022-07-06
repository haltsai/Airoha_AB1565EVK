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


###############################################################################
# feature option dependency
###############################################################################
ifeq ($(MTK_RACE_DUAL_CMD_ENABLE),y)
CFLAGS += -DMTK_RACE_DUAL_CMD_ENABLE
endif

ifeq ($(BSP_EXTERNAL_SERIAL_FLASH_ENABLE),y)
CFLAGS += -DMTK_RACE_BSP_FLASH_SUPPORT
endif


###################################################
# Sources
RACE_CMD_SRC = middleware/MTK/race_cmd

ifeq ($(AIR_RACE_CMD_ENABLE), y)
 RACE_CMD_FILES = $(RACE_CMD_SRC)/src/race_xport.c \
				  $(RACE_CMD_SRC)/src/race_util.c \
				  $(RACE_CMD_SRC)/src/race_storage_util.c \
				  $(RACE_CMD_SRC)/src/race_storage_access.c \
				  $(RACE_CMD_SRC)/src/race_noti.c \
				  $(RACE_CMD_SRC)/src/race_cmd_storage.c \
				  $(RACE_CMD_SRC)/src/race_cmd_nvdm.c \
				  $(RACE_CMD_SRC)/src/race_cmd_fota.c \
				  $(RACE_CMD_SRC)/src/race_fota_util.c \
				  $(RACE_CMD_SRC)/src/race_cmd_dsprealtime.c \
				  $(RACE_CMD_SRC)/src/race_cmd_ctrl_baseband.c \
				  $(RACE_CMD_SRC)/src/race_cmd_bluetooth.c \
				  $(RACE_CMD_SRC)/src/race_cmd_captouch.c \
				  $(RACE_CMD_SRC)/src/race_cmd_register.c \
				  $(RACE_CMD_SRC)/src/race_cmd.c \
				  $(RACE_CMD_SRC)/src/race_event.c \
				  $(RACE_CMD_SRC)/src/race_bt.c \
				  $(RACE_CMD_SRC)/src/race_timer.c \
				  $(RACE_CMD_SRC)/src/race_cmd_hostaudio.c \
				  $(RACE_CMD_SRC)/src/race_cmd_informational.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_agent.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_aws.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_packet.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_partner.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_ps.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_ps_list.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_ps_noti.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_recv.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_retry.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_trans.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_util.c \
				  $(RACE_CMD_SRC)/src/lpcomm/race_lpcomm_conn.c \
				  $(RACE_CMD_SRC)/src/race_cmd_find_me.c \
				  $(RACE_CMD_SRC)/src/race_cmd_bootreason.c \
				  $(RACE_CMD_SRC)/src/race_cmd_rofs.c \
				  $(RACE_CMD_SRC)/src/race_cmd_fcd_cmd.c \
				  $(RACE_CMD_SRC)/src/race_cmd_key_cmd.c \
				  $(RACE_CMD_SRC)/src/race_cmd_system_power.c \
				  $(RACE_CMD_SRC)/src/crc8.c \
				  $(RACE_CMD_SRC)/src/race_cmd_syslog.c \
				  $(RACE_CMD_SRC)/src/race_cmd_offline_log.c \
				  $(RACE_CMD_SRC)/src/race_cmd_online_log.c \
				  $(RACE_CMD_SRC)/src/race_cmd_online_log_2.c \
				  $(RACE_CMD_SRC)/src/race_cmd_system.c \
				  $(RACE_CMD_SRC)/src/race_cmd_version_code.c \
				  $(RACE_CMD_SRC)/src/race_cmd_i2c_master.c \
				  $(RACE_CMD_SRC)/src/race_cmd_spi_master.c \
				  $(RACE_CMD_SRC)/src/race_cmd_audio_loopback_test.c \
				  $(RACE_CMD_SRC)/src/race_cmd_cfu.c \
				  $(RACE_CMD_SRC)/src/cfu/race_cfu.c \
				  $(RACE_CMD_SRC)/src/cfu/race_cfu_builder.c \
				  $(RACE_CMD_SRC)/src/cfu/race_cfu_handler.c \
				  $(RACE_CMD_SRC)/src/race_cmd_le_audio.c
endif

C_FILES += $(RACE_CMD_FILES)

ifeq ($(AIR_RACE_SCRIPT_ENABLE),y)
C_FILES += $(RACE_CMD_SRC)/src/race_cmd_script.c
CFLAGS += -DAIR_RACE_SCRIPT_ENABLE
endif

ifneq ($(findstring $(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)$(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE),  y  y),)
C_FILES += $(RACE_CMD_SRC)/src/race_cmd_relay_cmd_cosys.c
C_FILES += $(RACE_CMD_SRC)/src/race_cmd_co_sys.c
CFLAGS += -DAIR_RACE_CO_SYS_ENABLE
else
C_FILES += $(RACE_CMD_SRC)/src/race_cmd_relay_cmd.c
endif

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/race_cmd/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/race_cmd/inc/lpcomm
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/race_cmd/inc/cfu
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/atci/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/port_service/inc
