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

BT_AIR_PATH = middleware/MTK/bt_air/src

ifeq ($(MTK_PORT_SERVICE_BT_ENABLE),y)
C_FILES  += $(BT_AIR_PATH)/ble_air/ble_air_low_power.c \
                   $(BT_AIR_PATH)/spp_air/spp_air_ServiceRecord.c \
                   $(BT_AIR_PATH)/spp_air/spp_air.c \
            $(BT_AIR_PATH)/ble_air/ble_air_service.c			 

ifeq ($(MTK_AIRUPDATE_ENABLE),y)
C_FILES  += $(BT_AIR_PATH)/airupdate/airupdate.c
endif
ifeq ($(MTK_GATT_OVER_BREDR_ENABLE),y)
C_FILES  += $(BT_AIR_PATH)/gatt_over_bredr_air/gatt_over_bredr_air.c
endif
endif
#################################################################################
#include path
#################################################################################
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/bt_air/inc
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/bt_air/inc/ble_air
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/bt_air/inc/spp_air
ifeq ($(MTK_AIRUPDATE_ENABLE),y)
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/bt_air/inc/airupdate
endif
ifeq ($(MTK_GATT_OVER_BREDR_ENABLE),y)
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/bt_air/inc/gatt_over_bredr_air
endif
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc 
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bt_callback_manager/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
ifeq ($(MTK_AWS_MCE_ENABLE),y)
ifneq ($(findstring $(SUPPORT_ROLE_HANDOVER_SERVICE)$(AIR_BT_ROLE_HANDOVER_ENABLE), y),)
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/bluetooth_service/bt_role_handover_service/inc
endif
endif
