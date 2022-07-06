###################################################
AUDIO_SRC = middleware/MTK/audio
TRANSMITTER_SRC = middleware/MTK/audio/audio_transmitter

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/audio_transmitter/inc
AUDIO_FILES += $(AUDIO_SRC)/audio_transmitter/src/audio_transmitter_playback.c
AUDIO_FILES += $(AUDIO_SRC)/audio_transmitter/src/audio_transmitter_control.c
AUDIO_FILES += $(AUDIO_SRC)/audio_transmitter/src/audio_transmitter_playback_port.c

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/audio_transmitter/inc/audio_transmitter_scenario_port


####################PORT###########################
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_gsensor.c
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_multi_mic_stream.c
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_gaming_mode.c
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_anc_monitor_stream.c
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_test.c
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_tdm.c
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_wired_audio.c
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_ble_audio.c
AUDIO_FILES += $(TRANSMITTER_SRC)/src/audio_transmitter_scenario_port/scenario_audio_hw_loopback.c
