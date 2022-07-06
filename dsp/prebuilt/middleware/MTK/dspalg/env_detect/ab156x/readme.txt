wind detection prebuilt module usage guide

Brief:          This module is the noise gate prebuilt library.

Usage:          XT-XCC:  For noise gaten, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/noise_gate/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         AIR_ANC_NOISE_GATE_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/noise_gate/inc

Dependency:     None

Notice:         None

Relative doc:   None

Example project:None

