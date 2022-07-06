Compander module usage guide

Brief:          This module is the AGC prebuild library implementation.

Usage:          GCC:  For AGC, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/agc/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_VOICE_AGC_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/agc/inc

Dependency:     None

Notice:         1) MTK_VOICE_AGC_ENABLE and MTK_BT_AGC_USE_PIC must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project:None