SMART_SRC = middleware/MTK/smart_charger

C_FILES += $(SMART_SRC)/src/chargersmartcase.c
C_FILES += $(SMART_SRC)/src/smartcase.c
C_FILES += $(SMART_SRC)/src/smartcharger.c
C_FILES += $(SMART_SRC)/src/smartcharger_1wire.c

  
#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/util/include 
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source
CFLAGS  += -I$(SOURCE_DIR)/middleware/mlog/include
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/include
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/smart_charger/inc

#################################################################################
#Enable the feature by configuring
CFLAGS += -DMTK_CHARGER_CASE
CFLAGS += -DMTK_SMART_ENABLED