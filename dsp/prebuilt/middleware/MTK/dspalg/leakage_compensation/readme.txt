Leakage Compensation prebuilt module usage guide

Brief:          This module is the Leakage Compensation prebuilt library.

Usage:          GCC:  For Leakage Compensation, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/leakage_compensation/module.mk
                      2) Module.mk provides different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_LEAKAGE_DETECTION_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/leakage_compensation/inc

Dependency:     None

Notice:         None

Relative doc:   None

Example project:None