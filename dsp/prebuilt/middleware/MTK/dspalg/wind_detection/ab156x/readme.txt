wind detection prebuilt module usage guide

Brief:          This module is the wind detection prebuilt library.

Usage:          XT-XCC:  For wind detection, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/wind_detection/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         AIR_ANC_WIND_DETECTION_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/wind_detection/inc

Dependency:     None

Notice:         None

Relative doc:   None

Example project:None

