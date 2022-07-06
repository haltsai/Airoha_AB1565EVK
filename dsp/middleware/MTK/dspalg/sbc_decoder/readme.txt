SBC Decoder module usage guide

Brief:          This module is the SBC Decoder implementation.

Usage:          GCC:  For SBC Decoder, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/sbc_decoder/module.mk
                      2) module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_BT_A2DP_SBC_ENABLE
                         MTK_BT_A2DP_SBC_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/sbc_decoder/inc
                      4) Add CODEC_DECODER_SBC into the feature table in dsp_sdk.c
                         to apply SBC decoder in the specified scenario, like stream_feature_list_a2dp[].

Dependency:     SBC decoder module is only effective when the SBC decoder prebuilt module also exists.

Notice:         1) MTK_BT_A2DP_SBC_ENABLE must be set as "y" on the specified XT-XCC/feature.mk.
                2) MTK_BT_A2DP_SBC_USE_PIC must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project:None