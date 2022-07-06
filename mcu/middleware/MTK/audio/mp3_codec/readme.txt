mp3 decoder module usage guide

Brief:          This module is the mp3 decoder for decoding mp3 format data.

Usage:          GCC:  For mp3 decoder, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                      2) Module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_MP3_DECODER_ENABLED
                         MP3_SW_DECODE_SUPPORT
                         MTK_AUDIO_MIXER_SUPPORT
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/mp3_codec/inc

                      For MP3_DECODER only,
                      1) Add the following moddule.mk in your GCC project Makefile.
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                      2) Set MTK_MP3_DECODER_ENABLED as "y" in specified GCC/feature.mk
                      3) Add the header file path in your GCC project Makefile:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/mp3_codec/inc


Dependency:     none.

Notice:         none.

Relative doc:   none.

Example project:none.

