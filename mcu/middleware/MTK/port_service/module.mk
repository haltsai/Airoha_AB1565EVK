
FEATURE ?= feature.mk
include $(FEATURE)

###################################################
# Sources
PORT_SERVICE_SRC = middleware/MTK/port_service
PORT_SERVICE_FILES = $(PORT_SERVICE_SRC)/src/serial_port_uart.c \
             $(PORT_SERVICE_SRC)/src/serial_port_usb.c \
             $(PORT_SERVICE_SRC)/src/serial_port.c \
			 $(PORT_SERVICE_SRC)/src/serial_port_para.c \


ifeq ($(MTK_PORT_SERVICE_BT_ENABLE),y)
PORT_SERVICE_FILES += $(PORT_SERVICE_SRC)/src/serial_port_bt.c
endif

ifneq ($(MTK_IAP2_VIA_MUX_ENABLE),y)
PORT_SERVICE_FILES += $(PORT_SERVICE_SRC)/src/serial_port_iap2.c
endif

C_FILES += $(PORT_SERVICE_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/port_service/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/usb/inc
ifeq ($(MTK_PORT_SERVICE_BT_ENABLE),y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_air/inc
endif
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/iap2/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sink/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_callback_manager
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_role_handover/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_connection_manager/inc
CFLAGS += -D__MTK_TARGET__
