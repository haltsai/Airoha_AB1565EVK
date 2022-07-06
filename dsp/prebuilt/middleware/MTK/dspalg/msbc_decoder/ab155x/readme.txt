mSBC Decoder/Encoder prebuilt module usage guide

Brief:          This module is the mSBC Decoder/Encoder prebuilt library.

Usage:          GCC:  For mSBC Decoder/Encoder, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/msbc_decoder/module.mk
                      2) Module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_BT_HFP_mSBC_ENABLE
                         MTK_BT_A2DP_MSBC_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/msbc_decoder/inc

Dependency:     None

Notice:         1) MTK_BT_HFP_mSBC_ENABLE must be set as "y" on the specified XT-XCC/feature.mk.
                2) MTK_BT_A2DP_MSBC_USE_PIC must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project:None