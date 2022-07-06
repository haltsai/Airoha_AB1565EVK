Fixed ratio software sample rate converter module usage guide

Brief:          This module is the Fixed ratio software sample rate converter (src_fixed_ratio) implementation.

Usage:          GCC:  For src_fixed_ratio, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/src_fixed_ratio/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         AIR_FIXED_RATIO_SRC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/src_fixed_ratio/inc
                      4) Add FUNC_SRC_FIXED_RATIO into the feature table in dsp_sdk.c to apply src_fixed_ratio in the specified scenario and then call src_fixed_ratio APIs to configure port settings.

Dependency:     None

Notice:         AIR_FIXED_RATIO_SRC must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project:None