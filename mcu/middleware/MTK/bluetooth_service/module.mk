###################################################
# Sources
###################################################

BT_COMP_SOURCE = middleware/MTK/bluetooth_service/src

C_FILES  += $(BT_COMP_SOURCE)/bt_device_manager_le.c \
            $(BT_COMP_SOURCE)/bt_gatts_service.c

ifeq ($(MTK_BLE_GAP_SRV_ENABLE), y)
C_FILES  += $(BT_COMP_SOURCE)/bt_gap_le_service.c \
            $(BT_COMP_SOURCE)/bt_gap_le_service_atci.c \
            $(BT_COMP_SOURCE)/bt_gap_le_service_utils.c
CFLAGS += -DMTK_BLE_GAP_SRV_ENABLE
endif
###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bluetooth_service/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_callback_manager/inc
ifeq ($(MTK_NVDM_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
endif
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc

 
