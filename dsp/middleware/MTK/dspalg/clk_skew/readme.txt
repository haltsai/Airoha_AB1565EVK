Clock Skew module usage guide

Brief:          This module is the Clock Skew (clk skew) implementation.

Usage:          GCC:  For clk skew, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/clk_skew/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_BT_CLK_SKEW_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/clk_skew/inc
                      4) Add FUNC_CLK_SKEW_DL (for a2dp downlink) / FUNC_CLK_SKEW_HFP_DL (for hfp downlink) / FUNC_CLK_SKEW_UL (for uplink) 
                         into the feature table in dsp_sdk.c to apply clk skew in the specified scenario, like stream_feature_list_hfp_downlink[].

Dependency:     Clock Skew module uses the protected clk skew library in $(ROOTDIR)/prebuilt/middleware/MTK/dspalg/clk_skew
                Make sure to include this library in the project Makefile, like dsp/project/ab155x_evk/apps/dsp0_headset_ref_design/XT-XCC/Makefile

Notice:         1) MTK_BT_CLK_SKEW_USE_PIC must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project:None