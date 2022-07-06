###################################################
# Sources
###################################################

AUDIO_MANAGER_SRC= middleware/MTK/audio_manager/src

AUDIO_MANAGER_SRV_FILES  = $(AUDIO_MANAGER_SRC)/audio_src_srv.c \
                           $(AUDIO_MANAGER_SRC)/audio_src_srv_reserve.c \
                           $(AUDIO_MANAGER_SRC)/audio_src_srv_state_mgr.c \
                           $(AUDIO_MANAGER_SRC)/bt_sink_srv_am_task.c \
                           $(AUDIO_MANAGER_SRC)/bt_sink_srv_ami.c \
                           $(AUDIO_MANAGER_SRC)/bt_sink_srv_audio_setting.c \

C_FILES += $(AUDIO_MANAGER_SRV_FILES)

ifeq ($(MTK_AUDIO_TUNING_ENABLED), y)
AUDIO_TUNNING_SRC = middleware/MTK/audio_manager/src
AUDIO_TUNNING_FILES = $(AUDIO_TUNNING_SRC)/bt_sink_srv_audio_tunning.c
C_FILES += $(AUDIO_TUNNING_FILES)
endif

###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio_manager/inc

