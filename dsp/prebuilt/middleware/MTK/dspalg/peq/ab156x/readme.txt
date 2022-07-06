PEQ prebuilt module usage guide

Brief:          This module is the PEQ prebuilt library.

Usage:          XT-XCC:  For PEQ, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/peq/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_PEQ_ENABLE
                         MTK_BT_PEQ_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/peq/inc

Dependency:     None

Notice:         MTK_BT_PEQ_USE_PIC must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project:None

