Audio Loopback Test module usage guide

Brief:          This module is the Audio Loopback Test implementation.

Usage:          GCC:  For Audio Loopback Test, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/audio_loopback_test/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_AUDIO_LOOPBACK_TEST_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/audio_loopback_test/inc
                      4) Add FUNC_AUDIO_LOOPBACK_TEST into the feature table in dsp_sdk.c to apply
                      audio loopback test in the specified scenario,
                         like stream_feature_list_audio_loopback_test[].

Dependency:     None

Notice:         None

Relative doc:   None

Example project:None
