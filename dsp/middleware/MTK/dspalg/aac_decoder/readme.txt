Airoha AAC Decoder module usage guide

Brief:          This module is the Airoha AAC Decoder implementation.

Usage:          GCC:  For Airoha AAC Decoder, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/aac_decoder/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_BT_A2DP_AAC_ENABLE
                         MTK_BT_A2DP_AAC_INHOUSE_ENABLE
                         MTK_BT_A2DP_AAC_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/aac_decoder/inc
                      4) Add CODEC_DECODER_AAC into the feature table in dsp_sdk.c to apply aac decoder in the specified scenario,
                         like stream_feature_list_a2dp[].

Dependency:     None

Notice:         1) MTK_BT_A2DP_AAC_ENABLE must be set as "y" on the specified XT-XCC/feature.mk.
                2) MTK_BT_A2DP_AAC_INHOUSE_ENABLE must be set as "y" on the specified XT-XCC/feature.mk.
                3) MTK_BT_A2DP_AAC_USE_PIC must be set as "y" on the specified DSP side XT-XCC/feature.mk.

Relative doc:   None

Example project:None
