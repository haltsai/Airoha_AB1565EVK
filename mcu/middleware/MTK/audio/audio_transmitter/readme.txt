audio transmitter module usage guide

Brief:          This module is the transmitter implementation, to transmit data from one user to another user actively or passively.

Usage:          GCC:  For audio transmitter, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                      2) Module.mk provide different options to enable or disable according profiles.
                         please configure these options on specified GCC/feature.mk:
                         MTK_AUDIO_TRANSMITTER_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/audio_transmitter/inc

Dependency:     none.

Notice:         none.

Relative doc:   none.

Example project:none.

