SBC codec module usage guide

Brief:          This module is a sbc codec for encoding the sbc format data.

Usage:          GCC:  For the SBC codec, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified GCC/feature.mk.
                         MTK_SBC_ENCODER_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/sbc_codec/inc

Dependency:     None.

Notice:         None.

Relative doc:   None.

Example project:None.
