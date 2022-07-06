Audio Loopback Test prebuilt module usage guide

Brief:          This module is the Audio Loopback Test prebuilt library.

Usage:          GCC:  For Audio Loopback Test, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/audio_loopback_test/module.mk
                      2) Module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_AUDIO_LOOPBACK_TEST_ENABLE
                      3) Add the header file path:
                          CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/audio_loopback_test/inc

Dependency:     None

Notice:         MTK_AUDIO_LOOPBACK_TEST_ENABLE must be configurate as y on specified GCC/feature.mk.

Relative doc:   None

Example project:None