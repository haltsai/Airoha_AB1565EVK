WAV codec module usage guide

Brief:          This module is a codec for decoding/encoding WAV format data.

Usage:          XT-XCC:  For the WAV codec, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_WAV_DECODER_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/wav_codec/inc

Dependency:     None.

Notice:         None.

Relative doc:   None.

Example project:None.
