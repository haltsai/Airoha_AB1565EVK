Software sample rate converter module usage guide

Brief:          This module is the Software sample rate converter (sw_src) implementation.

Usage:          GCC:  For sw_rc, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/sw_src/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         AIR_SOFTWARE_SRC_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/sw_src/inc
                      4) Add FUNC_SW_SRC into the feature table in dsp_sdk.c to apply sw_rc in the specified scenario and then call sw_src APIs to configure port settings.

Dependency:     None

Notice:         AIR_SOFTWARE_SRC_ENABLE must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project:None