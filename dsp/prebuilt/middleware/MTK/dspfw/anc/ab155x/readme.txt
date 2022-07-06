ANC module usage guide

Brief:          This module is the Active Noise Cancellation (ANC) implementation. It provides support for noise cancellation.

Usage:          XT-XCC:  For ANC, make sure to include the following:
                      1) Add the following module.mk for lib and its API header file:
                         include $(SOURCE_DIR)/prebuilt/middleware/MTK/dspfw/anc/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_ANC_ENABLE
                         MTK_DEQ_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/dspfw/anc/inc
                      4) Add FUNC_DEQ into the feature table in dsp_sdk.c to apply DEQ in the specified scenario, like stream_feature_list_a2dp[].
                         Add FUNC_DEQ_MUTE into the feature table in dsp_sdk.c if you don't want to apply DEQ in the specified scenario while the scenario is concurrent with ANC, like stream_feature_list_hfp_downlink[].

Dependency:     ANC DEQ uses PEQ to apply the EQ function and uses the second channel of feature buffer, therefore only the earbuds projects with ANC and PEQ can support ANC DEQ.

Notice:         1) MTK_DEQ_ENABLE can be 'y' only when MTK_ANC_ENABLE, MTK_PEQ_ENABLE are both 'y'.

Relative doc:   None

Example project: None
