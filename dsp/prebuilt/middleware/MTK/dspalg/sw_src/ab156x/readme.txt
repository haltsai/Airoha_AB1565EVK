Software sample rate converter prebuilt module usage guide

Brief:          This module is the Software sample rate converter prebuilt library.

Usage:          GCC:  For Software sample rate converter, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/sw_src/module.mk
                      2) module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         AIR_SOFTWARE_SRC_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/sw_src/inc

Dependency:     None.

Notice:         AIR_SOFTWARE_SRC_ENABLE must be configurate as y on specified GCC/feature.mk.

Relative doc:   None.

Example project:None.