wind detection module usage guide

Brief:          This module is the wind detection implementation, the module is reserved for future usage.

Usage:          GCC:  For wind detection, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/wind_detection/module.mk
                      2) module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         AIR_ANC_WIND_DETECTION_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/wind_detection/inc
                      4) Add FUNC_WIND_DETECT into the feature table in dsp_sdk.c
                         to apply wind detection in the specified scenario, like stream_featuremulti_anc_monitor[].

Dependency:     wind detection module is only effective when the wind detection prebuilt module also exists.

Notice:         None
                

Relative doc:   None

Example project:None