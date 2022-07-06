AAC decoder prebuilt module usage guide

Brief:          This module contains the third party library for AAC decoder and it is invented by Cadence.

Usage:          GCC:  For AAC decoder prebuilt module, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/third_party/dspalg/sbc_decoder/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_BT_A2DP_AAC_USE_PIC = y
                      3) Add the header file path:
                         AAC decoder prebuilt module is only effective when AAC decoder module also exists.
                         CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/third_party/dspalg/aac_decoder/inc
                      4) Add CODEC_DECODER_SBC into the feature table in dsp_sdk.c 
                         to apply AAC decoder in the specified scenario, like stream_feature_list_a2dp[].

Dependency:     AAC decoder prebuilt module is only effective when AAC decoder module also exists.

Notice:         1) MTK_BT_A2DP_AAC_USE_PIC must be set as "y" on the specified DSP side XT-XCC/feature.mk.

Relative doc:   None

Example project:None