AAC decoder module usage guide

Brief:          This module is the aac decoder implementation.

Usage:          GCC:  For aac decoder, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/third_party/dspalg/aac_decoder/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_BT_A2DP_AAC_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/dspalg/aac_decoder/inc
                      4) Add CODEC_DECODER_AAC into the feature table in dsp_sdk.c
                         to apply aac decoder in the specified scenario, like stream_feature_list_a2dp[].

Dependency:     AAC decoder can only be used when host support aac codec.
				MTK_BT_A2DP_AAC_ENABLE must be set as "y" on the specified CM4 side XT-XCC/feature.mk .

Notice:         1) MTK_BT_A2DP_AAC_USE_PIC must be set as "y" on the specified DSP side XT-XCC/feature.mk.

Relative doc:   None

Example project:None