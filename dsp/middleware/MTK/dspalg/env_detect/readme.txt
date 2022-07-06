environment detection module usage guide

Brief:          This module is the environment detection implementation, the module is reserved for future usage.

Usage:          GCC:  For environment detection, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/env_detect/module.mk
                      2) module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/env_detect/inc
                      4) Add FUNC_ENVIRONMENT_DETECTION into the feature table in dsp_sdk.c
                         to apply environment detection in the specified scenario, like stream_featuremulti_anc_monitor[].

Dependency:     environment detection module is only effective when the environment detection prebuilt module also exists.

Notice:         None


Relative doc:   None

Example project:None