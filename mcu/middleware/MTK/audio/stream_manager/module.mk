
AUDIO_STREAM_MANAGER_SRC = middleware/MTK/audio/stream_manager

C_FILES += $(AUDIO_STREAM_MANAGER_SRC)/src/audio_stream_manager_control.c
C_FILES += $(AUDIO_STREAM_MANAGER_SRC)/src/audio_stream_manager_port.c

CFLAGS  += -I$(SOURCE_DIR)/$(AUDIO_STREAM_MANAGER_SRC)/inc
