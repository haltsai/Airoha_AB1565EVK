audio codec stream usage guide

Brief:          This module is used to create a whole data stream in CM4 side, with WAV/OPUS/MP3.
                And now only Voice prompt uses it.
                It supports multi stream.
Usage:          GCC:  For audio codec st, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                         include $(SOURCE_DIR)/middleware/MTK/audio/audio_codec_stream/module.mk
                      2) Module.mk provide different options to enable or disable according profiles.
                         please configure these options on specified GCC/feature.mk:
                         AIR_OPUS_DECODER_ENABLE
                         MTK_WAV_DECODER_ENABLE
                         MTK_MP3_DECODER_ENABLED

Dependency:     none.

Notice:         Because the codec uses static memory, so you'd better enable only one of them (WAV/OPUS/MP3).

Relative doc:   none.

Example project:none.

