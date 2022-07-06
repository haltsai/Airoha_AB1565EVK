include $(SOURCE_DIR)/driver/chip/ab156x/src/GCC/module.mk

include $(SOURCE_DIR)/driver/board/$(BOARD_CONFIG)/hw_resource_assignment/module.mk

ifneq ($(wildcard $(strip $(SOURCE_DIR))/driver/chip/ab156x/src_core/),)
include $(SOURCE_DIR)/driver/chip/ab156x/src_core/GCC/module.mk
else
LIBS += $(SOURCE_DIR)/prebuilt/driver/chip/ab156x/lib/libhal_core_CM4_GCC.a
endif

ifneq ($(wildcard $(strip $(SOURCE_DIR))/driver/chip/ab156x/src_protected/),)
include $(SOURCE_DIR)/driver/chip/ab156x/src_protected/GCC/module.mk
else
LIBS += $(SOURCE_DIR)/prebuilt/driver/chip/ab156x/lib/libhal_protected_CM4_GCC.a
endif


CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
