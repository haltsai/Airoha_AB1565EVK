ifeq ($(MTK_BT_ENABLE), y)
###################################################
# Libs
###################################################

ifeq ($(MTK_BT_FAST_PAIR_ENABLE), y)
C_FILES += middleware/MTK/bt_fast_pair/src/bt_fast_pair_utility.c
C_FILES += middleware/MTK/bt_fast_pair/src/bt_fast_pair_spp.c
C_FILES += middleware/MTK/bt_fast_pair/src/bt_fast_pair_log.c
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_fast_pair/inc
CFLAGS += -D__BT_FAST_PAIR_ENABLE__

ifeq ($(AIR_SPOT_ENABLE), y)
FAST_PAIR_LIB = libbt_fast_pair_spot.a
else
FAST_PAIR_LIB = libbt_fast_pair.a
endif

export AIR_SPOT_ENABLE

ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/bt_fast_pair_protected/),)
LIBS += $(OUTPATH)/$(FAST_PAIR_LIB)
MODULE_PATH += $(SOURCE_DIR)/middleware/MTK/bt_fast_pair_protected/GCC
else
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/bluetooth/lib/$(FAST_PAIR_LIB)
endif

endif
endif
