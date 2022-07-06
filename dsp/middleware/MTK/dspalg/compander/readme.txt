Compander module usage guide

Brief:          This module is the Compander implementation.

Usage:          GCC:  For Compander, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/compander/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_BT_A2DP_CPD_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/compander/inc
                      4) Add FUNC_DRC (for a2dp) / FUNC_RX_WB_DRC (for hfp downlink) / FUNC_TX_WB_DRC (for uplink) 
                         into the feature table in dsp_sdk.c to apply compander in the specified scenario, like stream_feature_list_a2dp[].

Dependency:     None

Notice:         1) MTK_BT_A2DP_CPD_USE_PIC must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project:None