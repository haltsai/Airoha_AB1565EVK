-include $(SOURCE_DIR)/middleware/MTK/ble_ancs/module.mk
CFLAGS 	+= -I$(SOURCE_DIR)/prebuilt/middleware/MTK/ble_ancs/inc
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/ble_ancs/lib/libble_ancs.a
