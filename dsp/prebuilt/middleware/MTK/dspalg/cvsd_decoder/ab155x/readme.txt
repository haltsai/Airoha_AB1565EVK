cvsd decoder / encoder prebuilt module usage guide

Brief:          This module is the cvsd decoder / encoder prebuilt library.

Usage:          GCC:  For cvsd decoder / encoder, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/cvsd_decoder/module.mk
                      2) Module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_BT_HFP_CVSD_ENABLE
                         MTK_BT_A2DP_CVSD_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/cvsd_decoder/inc

Dependency:     None

Notice:         MTK_BT_HFP_CVSD_ENABLE & MTK_BT_A2DP_CVSD_USE_PIC must be configurate as y on specified GCC/feature.mk.

Relative doc:   None

Example project:None