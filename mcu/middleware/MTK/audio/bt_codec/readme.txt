BT Audio Codec control module usage guide

Brief:          This module is the Bluetooth Audio Codec control implementation including A2DP and HFP.

Usage:          GCC:  For Audio Port initialization, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified GCC/feature.mk.
                         MTK_BT_CODEC_ENABLED
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/bt_codec/inc

Dependency:     None.

Notice:         None.

Relative doc:   None.

Example project:None.
