ifeq ($(MTK_BT_DUO_ENABLE), y)
ifeq ($(MTK_BT_A2DP_VENDOR_ENABLE), y)
# Check if vendor codec exist in dsp side
VENDOR_LIB = $(strip $(SOURCE_DIR))/../dsp/prebuilt/middleware/third_party/dspalg/vendor_decoder/vend.flag
ifneq ($(VENDOR_LIB), $(wildcard $(VENDOR_LIB)))
VENDOR_LIB = $(strip $(SOURCE_DIR))/../dsp/prebuilt/middleware/third_party/dspalg/vendor_1_decoder/vend.flag
ifneq ($(VENDOR_LIB), $(wildcard $(VENDOR_LIB)))
#$(error If BT vendor codec doesn't exist in dsp side, user can't enable MTK_BT_A2DP_VENDOR_ENABLE)
else
CFLAGS   += -DMTK_BT_A2DP_VENDOR_ENABLE
CFLAGS   += -DMTK_BT_A2DP_VENDOR_CODEC_SUPPORT
ifeq ($(MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE), y)
CFLAGS   += -DMTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
endif
endif
else
CFLAGS   += -DMTK_BT_A2DP_VENDOR_ENABLE
CFLAGS   += -DMTK_BT_A2DP_VENDOR_CODEC_SUPPORT
ifeq ($(MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE), y)
CFLAGS   += -DMTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
endif
endif
endif

ifeq ($(MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT), y)
CFLAGS += -DMTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
endif

ifeq ($(MTK_BT_A2DP_VENDOR_2_ENABLE), y)
CFLAGS += -DMTK_BT_A2DP_VENDOR_2_ENABLE
endif

ifeq ($(MTK_AUDIO_SYNC_ENABLE), y)
CFLAGS += -DMTK_AUDIO_SYNC_ENABLE
endif

ifeq ($(AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT), y)
CFLAGS += -DAIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
endif

# BT sink source files
BT_SINK_SRV_SRC = middleware/MTK/sink/src
BT_SINK_SRV_FILES = $(BT_SINK_SRV_SRC)/bt_sink_srv.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_atci_cmd.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_common.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_state_notify.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_utils.c \

# Sink call related
BT_SINK_SRV_CALL_SRC = middleware/MTK/sink/src/call
BT_SINK_SRV_FILES += $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_call_audio.c \
                     $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_call.c \
                     $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_call_pseudo_dev.c \
                     $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_call_pseudo_dev_mgr.c \
                     $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_hf.c \
                     $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_hf_call_manager.c \
                     $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_hf_multipoint.c \
                     $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_pbapc.c
ifeq ($(MTK_AWS_MCE_ENABLE), y)
BT_SINK_SRV_FILES += $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_aws_mce_call.c \
		             $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_call_rho.c
endif
ifeq ($(MTK_BT_HSP_ENABLE), y)
BT_SINK_SRV_FILES += $(BT_SINK_SRV_CALL_SRC)/bt_sink_srv_hsp.c
endif

# Sink bt_music related
BT_SINK_SRV_BT_MUSIC_SRC = middleware/MTK/sink/src/bt_music
BT_SINK_SRV_FILES += $(BT_SINK_SRV_BT_MUSIC_SRC)/bt_sink_srv_a2dp.c \
                     $(BT_SINK_SRV_BT_MUSIC_SRC)/bt_sink_srv_a2dp_callback.c \
                     $(BT_SINK_SRV_BT_MUSIC_SRC)/bt_sink_srv_a2dp_state_machine.c \
                     $(BT_SINK_SRV_BT_MUSIC_SRC)/bt_sink_srv_avrcp.c \
                     $(BT_SINK_SRV_BT_MUSIC_SRC)/bt_sink_srv_music.c \
                     $(BT_SINK_SRV_BT_MUSIC_SRC)/bt_sink_srv_music_iot_device.c


ifeq ($(MTK_BT_SPEAKER_FEC_ENABLE), y)
CFLAGS += -DMTK_BT_SPEAKER_FEC_ENABLE
BT_SINK_SRV_FILES += $(BT_SINK_SRV_BT_MUSIC_SRC)/speaker_fec.c
endif

# Sink le audio related
ifeq ($(AIR_LE_AUDIO_ENABLE), y)
BT_SINK_SRV_LE_AUDIO_SRC = middleware/MTK/sink/src/le_audio
BT_SINK_SRV_FILES += $(BT_SINK_SRV_LE_AUDIO_SRC)/bt_sink_srv_le_cap.c\
                     $(BT_SINK_SRV_LE_AUDIO_SRC)/bt_sink_srv_le_cap_audio_manager.c\
                     $(BT_SINK_SRV_LE_AUDIO_SRC)/bt_sink_srv_le_cap_stream.c\
                     $(BT_SINK_SRV_LE_AUDIO_SRC)/bt_sink_srv_le.c\
                     $(BT_SINK_SRV_LE_AUDIO_SRC)/bt_sink_srv_le_music.c\
                     $(BT_SINK_SRV_LE_AUDIO_SRC)/bt_sink_srv_le_call.c\
					 $(BT_SINK_SRV_LE_AUDIO_SRC)/bt_sink_srv_le_volume.c
endif

# Sink avm_direct related
BT_SINK_SRV_AVM_DIRECT = middleware/MTK/sink/src/avm_direct
BT_SINK_SRV_FILES += $(BT_SINK_SRV_AVM_DIRECT)/avm_direct_util.c \

ifeq ($(MTK_AWS_MCE_ENABLE), y)
BT_SINK_SRV_FILES += $(BT_SINK_SRV_BT_MUSIC_SRC)/bt_sink_srv_aws_mce_a2dp.c \
                     $(BT_SINK_SRV_BT_MUSIC_SRC)/bt_sink_srv_music_rho.c
endif

# AWS MCE related
ifeq ($(MTK_AWS_MCE_ENABLE), y)
BT_SINK_SRV_FILES += $(BT_SINK_SRV_SRC)/bt_sink_srv_aws_mce.c
ifeq ($(MTK_PROMPT_SOUND_SYNC_ENABLE), y)
BT_SINK_SRV_FILES += $(BT_SINK_SRV_SRC)/bt_sink_srv_aws_mce_vp_sync.c
CFLAGS += -DMTK_PROMPT_SOUND_SYNC_ENABLE
endif
endif

ifneq ($(findstring $(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)$(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE),  y  y),)
BT_SINK_SRV_FILES += $(BT_SINK_SRV_SRC)/bt_sink_srv_dual_ant.c
CFLAGS += -DBT_SINK_DUAL_ANT_ENABLE
endif

C_FILES += $(BT_SINK_SRV_FILES)

# BT callback manager module
include $(SOURCE_DIR)/middleware/MTK/bt_callback_manager/module.mk

# BT device manager module
include $(SOURCE_DIR)/middleware/MTK/bluetooth_service/bt_device_manager_module.mk

# Audio manager
include $(SOURCE_DIR)/middleware/MTK/audio_manager/module.mk

# Include bt sink path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth_service/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sink/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sink/inc/call
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sink/inc/bt_music
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sink/inc/le_audio
#CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_sink/inc/audio_command_receiver
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_le_audio/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_le_audio/inc/bt_bap
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc

ifeq ($(MTK_AVM_DIRECT), y)
    CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/bt_codec/inc
else
    CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/bt_codec/inc
endif

CFLAGS += -mno-unaligned-access

endif

ifeq ($(MTK_WAV_DECODER_ENABLE), y)
CFLAGS += -DMTK_WAV_DECODER_ENABLE
endif

ifeq ($(AIR_MP3_DECODER_ENABLE), y)
CFLAGS += -DMTK_MP3_DECODER_ENABLED
endif

ifeq ($(MTK_AUDIO_GAIN_TABLE_ENABLE), y)
CFLAGS += -DMTK_AUDIO_GAIN_TABLE_ENABLE
endif

ifeq ($(MTK_AM_NOT_SUPPORT_STREAM_IN), y)
CFLAGS += -DMTK_AM_NOT_SUPPORT_STREAM_IN
endif

ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/avm_direct_protected/),)
include $(SOURCE_DIR)/middleware/MTK/avm_direct_protected/GCC/module.mk
else
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/bluetooth/lib/libavm_direct.a
endif

