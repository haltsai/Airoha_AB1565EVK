
###################################################

AUDIO_SRC = middleware/MTK/audio

ifeq ($(MTK_TEMP_REMOVE), y)
else
  ifeq ($(MTK_NVDM_ENABLE), y)
    AUDIO_FILES = $(AUDIO_SRC)/src/audio_middleware_api.c
    AUDIO_FILES += $(AUDIO_SRC)/src/audio_dsp_fd216_db_to_gain_value_mapping_table.c

    ifeq ($(IC_CONFIG),ab155x)
      AUDIO_FILES += $(AUDIO_SRC)/port/ab155x/src/audio_nvdm.c
    else ifeq ($(IC_CONFIG),mt2822)
      AUDIO_FILES += $(AUDIO_SRC)/port/mt2822/src/audio_nvdm.c
    else ifeq ($(IC_CONFIG),ab156x)
      AUDIO_FILES += $(AUDIO_SRC)/port/ab156x/src/audio_nvdm.c
    else ifeq ($(IC_CONFIG),am255x)
      AUDIO_FILES += $(AUDIO_SRC)/port/am255x/src/audio_nvdm.c
    else ifeq ($(IC_CONFIG),mt2533)
      AUDIO_FILES += $(AUDIO_SRC)/port/mt2533/src/audio_nvdm.c
    else
      AUDIO_FILES += $(AUDIO_SRC)/port/mt2523/src/audio_nvdm.c
    endif

  endif
endif

ifeq ($(MTK_AUDIO_AMR_ENABLED), y)
ifeq ($(IC_CONFIG),mt7687)
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_encoder.c
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_decoder.c
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/amr_codec/lib/arm_cm4/libamr.a
endif
ifeq ($(IC_CONFIG),mt7682)
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_encoder_7682.c
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_decoder_7682.c
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/amr_codec/lib/arm_cm4/libamr.a
endif
ifeq ($(IC_CONFIG),mt7686)
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_encoder_7682.c
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_decoder_7682.c
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/amr_codec/lib/arm_cm4/libamr.a
endif
ifeq ($(IC_CONFIG),aw7698)
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_encoder_7698.c
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_decoder_7698.c
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/amr_codec/lib/arm_cm4/libamr.a
endif
endif


ifeq ($(MTK_AUDIO_AAC_DECODER_ENABLED),y)
    ifeq ($(IC_CONFIG),mt7682)
      AUDIO_FILES  += $(AUDIO_SRC)/aac_codec/src/aac_decoder_api_7686.c
    else ifeq ($(IC_CONFIG),mt7686)
      AUDIO_FILES  += $(AUDIO_SRC)/aac_codec/src/aac_decoder_api_7686.c
	else ifeq ($(IC_CONFIG),aw7698)
      AUDIO_FILES  += $(AUDIO_SRC)/aac_codec/src/aac_decoder_api_7698.c
    else
      AUDIO_FILES  += $(AUDIO_SRC)/aac_codec/src/aac_decoder_api.c
    endif
    LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/aac_codec/lib/libheaac_decoder.a
endif

ifeq ($(MTK_PROMPT_SOUND_ENABLE), y)
  ifeq ($(MTK_AVM_DIRECT), y)
  else
    LIBS += $(SOURCE_DIR)/prebuilt/driver/board/component/audio/lib/arm_cm4/libblisrc.a

    AUDIO_FILES += $(AUDIO_SRC)/audio_mixer/src/audio_mixer.c
  endif

  AUDIO_FILES += $(AUDIO_SRC)/prompt_control/src/prompt_control.c
endif

# prompt sound dummy source module enable
ifeq ($(AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE), y)
    CFLAGS += -DAIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
endif

ifeq ($(MTK_SBC_ENCODER_ENABLE), y)
    LIBS += $(SOURCE_DIR)/middleware/MTK/audio/sbc_codec/lib/arm_cm4/libsbc_encoder.a
endif

ifeq ($(MTK_BT_A2DP_SOURCE_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/sbc_codec/src/sbc_codec.c
    LIBS += $(SOURCE_DIR)/middleware/MTK/audio/sbc_codec/lib/arm_cm4/libsbc_encoder.a
    LIBS += $(SOURCE_DIR)/prebuilt/driver/board/component/audio/lib/arm_cm4/libblisrc.a
endif

#bt codec driver source
ifeq ($(MTK_AVM_DIRECT), y)
    ifeq ($(MTK_BT_CODEC_ENABLED),y)
        AUDIO_FILES  += $(AUDIO_SRC)/bt_codec/src/bt_a2dp_codec.c
        AUDIO_FILES  += $(AUDIO_SRC)/bt_codec/src/bt_hfp_codec.c
    ifeq ($(AIR_BT_CODEC_BLE_ENABLED),y)
        CFLAGS += -DAIR_BT_CODEC_BLE_ENABLED
        AUDIO_FILES  += $(AUDIO_SRC)/bt_codec/src/bt_ble_codec.c
    endif
    endif
endif

ifeq ($(MTK_ANC_ENABLE), y)
    ifeq ($(IC_CONFIG),ab155x)
        AUDIO_FILES += $(AUDIO_SRC)/anc_control/src/anc_control.c
    endif
endif

# Audio sink line-in related
ifeq ($(MTK_LINEIN_PLAYBACK_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/linein_playback/src/linein_playback.c                       \
            $(AUDIO_SRC)/linein_playback/src/linein_control/audio_sink_srv_line_in_state_machine.c \
            $(AUDIO_SRC)/linein_playback/src/linein_control/audio_sink_srv_line_in_callback.c      \
            $(AUDIO_SRC)/linein_playback/src/linein_control/audio_sink_srv_line_in_playback.c      \
            $(AUDIO_SRC)/linein_playback/src/linein_control/audio_sink_srv_line_in_control.c
endif

# pure linein playback check
ifeq ($(MTK_PURE_LINEIN_PLAYBACK_ENABLE),y)
    ifeq ($(MTK_LINEIN_PLAYBACK_ENABLE),n)
    $(error IF you select MTK_PURE_LINEIN_PLAYBACK_ENABLE, you also need select MTK_LINEIN_PLAYBACK_ENABLE)
    endif
    CFLAGS += -DHAL_AUDIO_POWER_SLIM_ENABLED
    CFLAGS += -DHAL_PURE_LINEIN_PLAYBACK_ENABLE
endif

ifeq ($(MTK_RECORD_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/record_control/src/record_control.c
endif

AIR_AUDIO_DUMP_ENABLE ?= $(MTK_AUDIO_DUMP_ENABLE)
ifeq ($(AIR_AUDIO_DUMP_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/record_control/audio_dump/src/audio_dump.c
endif

AUDIO_FILES += $(AUDIO_SRC)/src/audio_log.c

ifeq ($(MTK_ANC_SURROUND_MONITOR_ENABLE),y)
    AUDIO_FILES += $(AUDIO_SRC)/anc_monitor/src/anc_monitor.c
endif

C_FILES += $(AUDIO_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/inc

ifeq ($(IC_CONFIG),ab155x)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/ab155x/inc
else ifeq ($(IC_CONFIG),mt2822)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/mt2822/inc
else ifeq ($(IC_CONFIG),ab156x)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/ab156x/inc
else ifeq ($(IC_CONFIG),am255x)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/am255x/inc
else ifeq ($(IC_CONFIG),mt2533)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/mt2533/inc
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/mt2533/inc/mt2533_external_dsp_profile
else
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/mt2523/inc
endif



ifeq ($(AIR_AUDIO_MP3_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/mp3_codec/inc
else ifeq ($(AIR_MP3_DECODER_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/mp3_codec/inc
endif

ifeq ($(MTK_AUDIO_AMR_ENABLED), y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/amr_codec/inc
endif

ifeq ($(MTK_AUDIO_AAC_DECODER_ENABLED), y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/aac_codec/inc
endif

ifeq ($(MTK_PROMPT_SOUND_ENABLE), y)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/audio_mixer/inc
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/prompt_control/inc
  CFLAGS += -I$(SOURCE_DIR)/prebuilt/driver/board/component/audio/inc
endif

ifeq ($(MTK_SBC_ENCODER_ENABLE), y)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/sbc_codec/inc
endif

ifeq ($(MTK_BT_A2DP_SOURCE_ENABLE), y)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/sbc_codec/inc
  CFLAGS += -I$(SOURCE_DIR)/prebuilt/driver/board/component/audio/inc
endif

#bt codec driver source
ifeq ($(MTK_AVM_DIRECT), y)
    ifeq ($(MTK_BT_CODEC_ENABLED),y)
        CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/bt_codec/inc
        CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
        CFLAGS  += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/bluetooth/inc
        ifeq ($(MTK_BT_A2DP_SOURCE_ENABLE), y)
            CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/sbc_codec/inc
        endif
    endif

    ifeq ($(MTK_BT_A2DP_SOURCE_ENABLE), y)
        CFLAGS += -DMTK_BT_A2DP_SOURCE_SUPPORT
    endif
endif

ifeq ($(MTK_ANC_ENABLE), y)
    ifeq ($(IC_CONFIG),ab155x)
        CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/anc_control/inc
    endif
endif

# Include audio sink path
ifeq ($(MTK_LINEIN_PLAYBACK_ENABLE), y)
    CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/linein_playback/inc
    CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/linein_playback/inc/linein_control
    CFLAGS  += -DMTK_LINE_IN_ENABLE
endif

ifeq ($(MTK_RECORD_ENABLE), y)
  CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/record_control/inc
endif

ifeq ($(AIR_AUDIO_DUMP_ENABLE), y)
  CFLAGS  += -DMTK_AUDIO_DUMP_ENABLE
  CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/record_control/audio_dump/inc
endif

ifeq ($(MTK_LEAKAGE_DETECTION_ENABLE), y)
  CFLAGS  += -DMTK_LEAKAGE_DETECTION_ENABLE
  CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/leakage_detection/inc
  AUDIO_FILES += $(AUDIO_SRC)/leakage_detection/src/leakage_detection_control.c
endif

ifeq ($(MTK_USER_TRIGGER_FF_ENABLE), y)
  CFLAGS  += -DMTK_USER_TRIGGER_FF_ENABLE
  ifeq ($(IC_CONFIG),ab156x)
    CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/user_trigger_adaptive_ff/inc
    AUDIO_FILES += $(AUDIO_SRC)/user_trigger_adaptive_ff/src/user_trigger_adaptive_ff.c
  endif
endif

# Audio sink USB_AUDIO_PLAYBACK related
ifeq ($(MTK_USB_AUDIO_PLAYBACK_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/usb_audio_playback/src/usb_audio_playback.c
    AUDIO_FILES += $(AUDIO_SRC)/usb_audio_playback/src/usb_audio_device.c
    AUDIO_FILES += $(AUDIO_SRC)/usb_audio_playback/src/usb_audio_control.c

    CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/usb_audio_playback/inc

    CFLAGS  += -DMTK_USB_AUDIO_PLAYBACK_ENABLE

ifeq ($(MTK_ULL_DONGLE_LOCAL_PLAYBACK_ENABLE),y)
    CFLAGS  += -DMTK_ULL_DONGLE_LOCAL_PLAYBACK_ENABLE
endif
endif

# Audio sink USB_AUDIO_RECORD
ifeq ($(MTK_USB_AUDIO_MICROPHONE), y)
ifeq ($(MTK_USB_AUDIO_RECORD_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/usb_audio_record/src/usb_audio_record.c
    CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/usb_audio_record/inc
endif
endif
ifeq ($(MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_ENABLE), y)
  CFLAGS  += -DMTK_VENDOR_STREAM_OUT_VOLUME_TABLE_ENABLE
  ifneq ($(MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_MAX_MODE_NUM),)
  CFLAGS += -DMTK_VENDOR_STREAM_OUT_VOLUME_TABLE_MAX_MODE_NUM=$(MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_MAX_MODE_NUM)
  endif
endif

ifeq ($(MTK_VENDOR_SOUND_EFFECT_ENABLE), y)
  CFLAGS  += -DMTK_VENDOR_SOUND_EFFECT_ENABLE
endif

ifeq ($(MTK_AUDIO_CODEC_MANAGER_ENABLE), y)
  CFLAGS += -DMTK_AUDIO_CODEC_MANAGER_ENABLE
  CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/codec_manager/inc
  CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/codec_manager/memory/inc
  AUDIO_FILES += $(AUDIO_SRC)/codec_manager/src/audio_codec_manager.c
  AUDIO_FILES += $(AUDIO_SRC)/codec_manager/memory/src/audio_codec_none_codec.c
ifeq ($(MTK_SBC_ENCODER_ENABLE), y)
  CFLAGS += -DMTK_SBC_ENCODER_ENABLE
  CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/codec_manager/sbc/inc
  AUDIO_FILES += $(AUDIO_SRC)/codec_manager/sbc/src/audio_codec_sbc_encoder.c
endif
ifeq ($(MTK_OPUS_ENCODER_ENABLE), y)
  CFLAGS += -DMTK_OPUS_ENCODER_ENABLE
  CFLAGS += -DHAVE_CONFIG_H -DFIXED_POINT=1 -DCUSTOM_MODES -DCELT_C -DNONTHREADSAFE_PSEUDOSTACK=1 -DOPUS_BUILD=1 -DDISABLE_FLOAT_API=1
  CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/codec_manager/opus/inc
  AUDIO_FILES += $(AUDIO_SRC)/codec_manager/opus/src/audio_codec_opus_encoder.c
ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/third_party/audio_codec/opus_codec_source/opus_encoder/),)
  include $(SOURCE_DIR)/middleware/third_party/audio_codec/opus_codec_source/opus_encoder/GCC/module.mk
else
  LIBS += $(SOURCE_DIR)/prebuilt/middleware/third_party/audio/opus_codec/opus_encoder/libopus_encoder.a
endif
endif
endif

ifeq ($(MTK_AUDIO_TRANSMITTER_ENABLE), y)
  CFLAGS  += -DMTK_AUDIO_TRANSMITTER_ENABLE
  include $(SOURCE_DIR)/middleware/MTK/audio/audio_transmitter/module.mk
endif

ifeq ($(MTK_MULTI_MIC_STREAM_ENABLE), y)
  CFLAGS  += -DMTK_MULTI_MIC_STREAM_ENABLE
endif

ifeq ($(MTK_ANC_SURROUND_MONITOR_ENABLE),y)
  CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio/anc_monitor/inc
endif

# wav decoder
ifeq ($(MTK_WAV_DECODER_ENABLE),y)
  CFLAGS += -DMTK_WAV_DECODER_ENABLE
  LIBS += $(SOURCE_DIR)/middleware/MTK/audio/wav_codec/lib/arm_cm4/libwav_codec.a
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/wav_codec/inc
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/codec_manager/wav/inc
  C_FILES += $(AUDIO_SRC)/codec_manager/wav/src/audio_codec_wav_decoder.c
  C_FILES += $(AUDIO_SRC)/wav_codec/src/wav_codec.c
endif
# mp3 decoder
ifeq ($(AIR_MP3_DECODER_ENABLE),y)
  CFLAGS += -DAIR_MP3_DECODER_ENABLE
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/codec_manager/mp3/inc
  C_FILES += $(AUDIO_SRC)/codec_manager/mp3/src/audio_codec_mp3_decoder.c
  LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/mp3_codec/lib/arm_cm4/libmp3dec.a
endif

# opus decoder
ifeq ($(AIR_OPUS_DECODER_ENABLE),y)
  CFLAGS += -DAIR_OPUS_DECODER_ENABLE
  LIBS += $(SOURCE_DIR)/prebuilt/middleware/third_party/audio/opus_codec/opus_decoder/libopus_vp.a
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/codec_manager/opus/inc
  C_FILES += $(AUDIO_SRC)/codec_manager/opus/src/audio_codec_opus_decoder.c
endif

ifeq ($(MTK_PROMPT_SOUND_ENABLE)_$(MTK_AUDIO_CODEC_MANAGER_ENABLE), y_y)
  include  $(SOURCE_DIR)/middleware/MTK/audio/stream_manager/module.mk
  CFLAGS += -DAIR_STREAM_MANAGER_ENABLE
  CFLAGS += -DAIR_PROMPT_SOUND_LINERBUFFER_ENABLE
endif
AUDIO_FILES += $(AUDIO_SRC)/src/audio_codec.c

ifeq ($(AIR_ADVANCED_PASSTHROUGH_ENABLE),y)
  AUDIO_FILES +=  $(AUDIO_SRC)/psap_common/src/psap_common.c
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/psap_common/inc
endif

