PEQ module usage guide

Brief:          This module is the parametric equalization (PEQ) implementation for audio. It provides support for cutting or boosting a range of frequencies with filters.

Usage:          XT-XCC:  For PEQ, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/peq/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_PEQ_ENABLE
                         MTK_BT_PEQ_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/peq/inc
                      4) Add FUNC_PEQ into the feature table in dsp_sdk.c to apply PEQ in the specified scenario, like stream_feature_list_a2dp[].

Dependency:     PEQ uses lt_clk_skew_get_asi_threshold() in $(SOURCE_DIR)/middleware/MTK/dspalg/clk_skew/src/long_term_clk_skew.c
                for changing PEQ coefficient synchronization in earbuds project.

Notice:         1) MTK_BT_PEQ_USE_PIC must be set as "y" on the specified XT-XCC/feature.mk.
                2) The maximum number of bands is defined in peq_interface.h.
                   MAX_BANDS is the limit of the PEQ library.
                   FW_MAX_BANDS is the limit of the PEQ control layer in CM4.
                   min(MAX_BANDS, FW_MAX_BANDS) is the maximum number of bands that this SDK supports.
                3) FW_MAX_ELEMENT defined in peq_interface.h means this SDK supports FW_MAX_ELEMENT kinds of sampling rate.

Relative doc:   None

Example project: None
