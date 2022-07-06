Airoha Leakage Compensation module usage guide

Brief:          This module is the Airoha Leakage compensation implementation.

Usage:          GCC:  For Airoha Leakage compensation, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/leakage_compensation/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_LEAKAGE_DETECTION_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/leakage_compensation/inc
                      4) Add FUNC_LEAKAGE_COMPENSATION into the feature table in dsp_sdk.c to apply leakage compensation in the specified scenario,
                         like stream_feature_list_leakage_compensation[].

Dependency:     Leakage compensation module uses the protected leakage compensation library in $(ROOTDIR)/prebuilt/middleware/MTK/dspalg/leakage_compensation
                Make sure to include this library in the project Makefile, like dsp/project/ab155x_evk/apps/dsp0_headset_ref_design/XT-XCC/Makefile

Notice:         None

Relative doc:   None

Example project:None
