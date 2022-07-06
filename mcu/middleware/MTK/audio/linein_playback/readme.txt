linein playback module usage guide

Brief:          This module is line-in playback implementation.

Usage:          GCC:  For line-in playback, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                      2) Module.mk provides different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_LINEIN_PLAYBACK_ENABLE
                      3) Add the header file path:
                         CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/linein_playback/inc
                         CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/linein_playback/inc/linein_control

Dependency:     none.

Notice:         none.

Relative doc:   none.

Example project:none.

