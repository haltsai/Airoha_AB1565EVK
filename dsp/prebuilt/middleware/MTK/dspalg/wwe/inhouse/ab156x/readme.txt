WWE prebuilt module usage guide

Brief:          This module is the WWE prebuilt library.

Usage:          XT-XCC:  For WWE, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/wwe/module.mk
                      2) module.mk provides different options to enable or disable according to the profiles, please configure these options on specified GCC/feature.mk:
                         MTK_WWE_ENABLE
                         MTK_WWE_AMA_ENABLE
                         MTK_WWE_AMA_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/wwe/inc

Dependency:     None

Notice:         None

Relative doc:   None

Example project:None