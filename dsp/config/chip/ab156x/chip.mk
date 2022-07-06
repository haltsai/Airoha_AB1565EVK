XTENSA_CORE               = AB1568_i64B_d32B_512K
PRODUCT_VERSION           = 1565
MTK_SYSLOG_VERSION_2     ?= y
MTK_SYSLOG_SUB_FEATURE_STRING_LOG_SUPPORT = y
MTK_SYSLOG_SUB_FEATURE_BINARY_LOG_SUPPORT = y
MTK_SYSLOG_SUB_FEATURE_USB_ACTIVE_MODE = y
MTK_SYSLOG_SUB_FEATURE_OFFLINE_DUMP_ACTIVE_MODE = y
MTK_DEBUG_PLAIN_LOG_ENABLE            = n
MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE  ?= n
MTK_CPU_NUMBER_1                      ?= y
MTK_MUX_ENABLE                        ?= y
FPGA_ENV                 ?= n
MTK_AUDIO_LOOPBACK_TEST_ENABLE ?= y
AIR_AUDIO_LDO_OUTPUT_0_85V_ENABLE ?= y
MTK_AUDIO_TRANSMITTER_ENABLE ?= y
IOT_SDK_XTENSA_VERSION                 := 809
AIR_CPU_MCPS_PRIORING_ENABLE   ?= n
MTK_BT_A2DP_ENABLE ?= y
MTK_BT_A2DP_AAC_ENABLE ?= y
MTK_BT_A2DP_SBC_ENABLE ?= y
AIR_VOICE_NR_ENABLE    ?= y
AIR_BT_HFP_MSBC_ENABLE ?= y
AIR_BT_HFP_CVSD_ENABLE ?= y
AIR_AUDIO_HARDWARE_ENABLE ?= y

ifeq ($(IC_CONFIG),ab156x)
CCFLAG += -DAB156X
endif
ifeq ($(IC_TYPE),ab1565)
CCFLAG += -DAB1565
endif
ifeq ($(IC_TYPE),ab1568)
CCFLAG += -DAB1568
endif


ifeq ($(IC_TYPE),ab156x)
CCFLAG += -DAB1565
endif

CCFLAG += -DPRODUCT_VERSION=$(PRODUCT_VERSION)
CCFLAG += -DCORE_DSP0
ASFLAG += -DCORE_DSP0
CCFLAG += -D$(TARGET)_BOOTING
ifeq ($(FPGA_ENV),y)
CCFLAG += -DFPGA_ENV
endif

# Link and Compile Configuration

#CCFLAG      :=
CCFLAG      +=  -g
CCFLAG      +=  -W
CCFLAG      +=  -Wall
CCFLAG      +=  -Os
CCFLAG      +=  -INLINE:requested
CCFLAG      +=  -mlongcalls
CCFLAG      +=  -std=gnu99
CCFLAG      +=  -ffunction-sections -fdata-sections -mtext-section-literals

#ASFLAG      :=
ASFLAG      +=  -W
ASFLAG      +=  -Wall
ASFLAG      +=  -g
ASFLAG      +=  -mlongcalls --text-section-literals

#LDFLAG      :=
LDFLAG      +=  --gc-sections
LDFLAG      +=  --no-relax
LDFLAG      +=  -wrap=printf
LDFLAG	    += -u _UserExceptionVector
LDFLAG	    += -u _KernelExceptionVector
LDFLAG	    += -u _DoubleExceptionVector

##
## MTK_DEBUG_PLAIN_LOG_ENABLE
## Brief:       This option is used to force log display with plain style.
## Usage:       Enable the feature by configuring it as y.
##              y : log display with plain style
##              n : log display with race style, need pc logging tool support
## Path:        kernel/service/syslog
## Dependency:  None
## Notice:      None
## Related doc :None
##
ifeq ($(MTK_DEBUG_PLAIN_LOG_ENABLE),y)
CCFLAG += -DMTK_DEBUG_PLAIN_LOG_ENABLE
endif

##
## AIR_CPU_MCPS_PRIORING_ENABLE
## Brief:       This option is used to enable task profiling.
## Usage:       Enable the feature by configuring it as y.
##              y : it will show task/heap status and cpu mips
##              n : it will not show task/heap status and cpu mips
## Path:        kernel/service/rtos
## Dependency:  None
## Notice:      None
## Related doc :None
##
ifeq ($(AIR_CPU_MCPS_PRIORING_ENABLE),y)
CCFLAG += -DAIR_CPU_MCPS_PRIORING_ENABLE
endif


##
## MTK_DEBUG_LEVEL
## Brief:       This option is to configure system log debug level.
## Usage:       The valid values are empty, error, warning, info, debug, and none.
##              The setting will determine whether a debug log will be compiled.
##              However, the setting has no effect on the prebuilt library.
##              empty   : All debug logs are compiled.
##              error   : Only error logs are compiled.
##              warning : Only warning and error logs are compiled.
##              info    : Only info, warning, and error logs are compiled.
##              debug   : All debug logs are compiled.
##              none    : All debugs are disabled.
## Path:        kernel/service
## Dependency:  None
## Notice:      None
## Realted doc: Please refer to doc/LinkIt_for_RTOS_System_Log_Developers_Guide.pdf
##
ifeq ($(MTK_DEBUG_LEVEL),)
CCFLAG += -DMTK_DEBUG_LEVEL_DEBUG
CCFLAG += -DMTK_DEBUG_LEVEL_INFO
CCFLAG += -DMTK_DEBUG_LEVEL_WARNING
CCFLAG += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),error)
CCFLAG += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),warning)
CCFLAG += -DMTK_DEBUG_LEVEL_WARNING
CCFLAG += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),info)
CCFLAG += -DMTK_DEBUG_LEVEL_INFO
CCFLAG += -DMTK_DEBUG_LEVEL_WARNING
CCFLAG += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),debug)
CCFLAG += -DMTK_DEBUG_LEVEL_DEBUG
CCFLAG += -DMTK_DEBUG_LEVEL_INFO
CCFLAG += -DMTK_DEBUG_LEVEL_WARNING
CCFLAG += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),printf)
CCFLAG += -DMTK_DEBUG_LEVEL_PRINTF
endif
ifeq ($(MTK_DEBUG_LEVEL),none)
CCFLAG += -DMTK_DEBUG_LEVEL_NONE
endif

##
## MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE
## Brief:       This option is to enable runtime log and crash context save in flash feature.
## Usage:       Enable the feature by configuring it as y.
##              y : save runtime logging registers and all memory in coredump format
##              n   : no effect
## Path:        kernel/service/src_core
## Dependency:  flash driver
## Notice:      Reserve flash blocks to store runtime log and dumped data
## Related doc :None
##
ifeq ($(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE),y)
CCFLAG += -DMTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE
endif
##

##
## MTK_USB_DEMO_ENABLED
## Brief:       This option is to enable USB device feature.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/usb/
## Dependency:  None
## Notice:      None
## Related doc :None
##
ifeq ($(MTK_USB_DEMO_ENABLED),y)
CCFLAG += -DMTK_USB_DEMO_ENABLED
endif

##
## AIR_BT_CODEC_BLE_ENABLED
## Brief:       This option is to enable le audio feature.
## Usage:       Enable enable prompt sound feature by configuring it as y.
##              y : enable prompt sound feature.
##              n : not enable prompt sound feature.
## Path:        middleware\MTK\stream, middleware\MTK\dspfw, middleware\MTK\dspalg
## Dependency:  None.
## Notice:      None.
## Realted doc: None.
##
ifeq ($(AIR_BT_CODEC_BLE_ENABLED),y)
CCFLAG += -DAIR_BT_CODEC_BLE_ENABLED
endif

##
## AIR_BT_CODEC_BLE_V2_ENABLED
## Brief:       This option is to enable le audio codec feature.
## Usage:       Enable enable le audio codec feature by configuring it as y.
##              y : enable le audio codec feature.
##              n : not enable le audio codec feature.
## Path:        middleware\MTK\dspalg\lc3_codec
## Dependency:  None.
## Notice:      None.
## Realted doc: None.
##
ifeq ($(AIR_BT_CODEC_BLE_V2_ENABLED),y)
CCFLAG += -DAIR_BT_CODEC_BLE_V2_ENABLED
endif

##
## MTK_PROMPT_SOUND_ENABLE
## Brief:       This option is to enable prompt sound feature.
## Usage:       Enable enable prompt sound feature by configuring it as y.
##              y : enable prompt sound feature.
##              n : not enable prompt sound feature.
## Path:        middleware\MTK\stream
## Dependency:  None.
## Notice:      None.
## Realted doc: None.
##
ifeq ($(MTK_PROMPT_SOUND_ENABLE),y)
CCFLAG += -DMTK_PROMPT_SOUND_ENABLE
CCFLAG += -DAIR_VP_SHAREBUFFER_SIZE_12KB_ENABLE
endif

##
## AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
## Brief:       This option is to dedicate vp memory in framework.
## Usage:       Enable enable prompt sound feature by configuring it as y.
##              y : dedicate vp memory in framework.
##              n : not dedicate vp memory in framework.
## Path:        middleware\MTK\stream
## Dependency:  None.
## Notice:      None.
## Realted doc: None.
##
ifeq ($(AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE),y)
CCFLAG += -DAIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
endif

##
## AIR_I2S_SLAVE_ENABLE
## Brief:       This option is to open i2s slave rx recording.
## Usage:       Enable the i2s slave driver by configuring it as y.
##              y : open i2s slave driver.
##              n : not open i2s slave driver.
## Path:        middleware/MTK/dspfw/port/chip/ab156x/src/dsp_lower_layer/dsp_drv
## Dependency:  None
## Notice:      None
## Realted doc: None
##
ifeq ($(AIR_I2S_SLAVE_ENABLE),y)
CCFLAG += -DAIR_I2S_SLAVE_ENABLE
endif

##
## PRELOADER_ENABLE
## Brief:       This option is to enable and disable preload pisplit features(dynamic to load PIC libraries)
## Usage:       If the value is "y", the PRELOADER_ENABLE compile option will be defined. You must also include the kernel/service/pre_libloader/dsp0/module.mk in your Makefile before setting the option to "y".
## Path:        kernel/service/pre_libloader
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(PRELOADER_ENABLE),y)
CCFLAG += -DPRELOADER_ENABLE
ASFLAG += -DPRELOADER_ENABLE

##
## DSP0_PISPLIT_DEMO_LIBRARY
## Brief:       This option is to enable and disable the demo of DSP0 PIC library
## Usage:       If the value is "y", the DSP0_PISPLIT_DEMO_LIBRARY compile option will be defined. This is a sub-feature option of PRELOADER_ENABLE.
## Path:        kernel/service/pre_libloader/dsp0/dsp0_pic_demo_portable
## Dependency:  PRELOADER_ENABLE
## Notice:      None
## Relative doc:None
##
ifeq ($(DSP0_PISPLIT_DEMO_LIBRARY),y)
CCFLAG += -DDSP0_PISPLIT_DEMO_LIBRARY
endif
endif

##
## CCCI_ENABLE
## Brief:       This option is to enable and disable CCCI(Cross Core communication Interface)
## Usage:       If the value is "y", the CCCI_ENABLE compile option will be defined.
## Path:        kernel/service/ccci
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(CCCI_ENABLE),y)
CCFLAG += -DCCCI_ENABLE
endif


##
## LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
## Brief:       This option is to enable for Audio HQA verification.
## Usage:       If the value is "y",  the LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA),y)
CCFLAG += -DLINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
endif

##
## ANALOG_OUTPUT_CLASSD_ENABLE
## Brief:       This option is to enable for default setting to class-d.
## Usage:       If the value is "y",  the ANALOG_OUTPUT_CLASSD_ENABLE option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(ANALOG_OUTPUT_CLASSD_ENABLE),y)
CCFLAG += -DANALOG_OUTPUT_CLASSD_ENABLE
endif

##
## MTK_LEAKAGE_DETECTION_ENABLE
## Brief:       This option is to enable for Leakage detection
## Usage:       If the value is "y",  the MTK_LEAKAGE_DETECTION_ENABLE option will be defined.
## Path:        middleware/MTK/dspalg/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_LEAKAGE_DETECTION_ENABLE),y)
CCFLAG += -DMTK_LEAKAGE_DETECTION_ENABLE
endif

##
## MTK_USER_TRIGGER_FF_ENABLE
## Brief:       This option is to enable for User triggered adaptive ANC
## Usage:       If the value is "y",  the MTK_USER_TRIGGER_FF_ENABLE option will be defined.
## Path:        middleware/MTK/dspalg/
## Dependency:  ANC
## Notice:      None
## Relative doc:None
##

##
## MTK_ANC_ENABLE
## Brief:       This option is to enable Active Noise Cancellation (ANC) main function.
## Usage:       Enable ANC feature by configuring it as y.
##              y : enable ANC feature.
##              n : not enable ANC feature.
## Path:        dsp/prebuilt/middleware/MTK/dspfw/anc/$(IC_TYPE)
## Dependency:  None.
## Notice:      None.
## Realted doc: None.
##
ifeq ($(MTK_USER_TRIGGER_FF_ENABLE),y)
ifeq ($(MTK_ANC_ENABLE),y)
CCFLAG += -DMTK_USER_TRIGGER_FF_ENABLE
CCFLAG += -DMTK_USER_TRIGGER_ADAPTIVE_FF_V2
endif
endif

##
## AIR_FIXED_SIDETONE_SOURCE_TO_I2S_RCH_ENABLE
## Brief:       This option is to enable sidetone source from i2s master R channel
## Usage:       If the value is "y",  the AIR_FIXED_SIDETONE_SOURCE_TO_I2S_RCH_ENABLE option will be defined.
## Path:        driver/chip/
## Dependency:  Sidetone
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_FIXED_SIDETONE_SOURCE_TO_I2S_RCH_ENABLE),y)
CCFLAG += -DAIR_FIXED_SIDETONE_SOURCE_TO_I2S_RCH_ENABLE
endif

##
## AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
## Brief:       This option is to enable amp to apply L/R offset analog gain.
## Usage:       If the value is "y",  the AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE option will be defined.
## Path:        driver/chip/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE),y)
CCFLAG += -DAIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
endif

##
## AIR_UL_FIX_SAMPLING_RATE_48K
## Brief:       This option is to enable UL sampling rate fixed 48kHz.
## Usage:       If the value is "y",  the AIR_UL_FIX_SAMPLING_RATE_48K option will be defined.
## Path:        driver/chip/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_UL_FIX_SAMPLING_RATE_48K),y)
CCFLAG += -DAIR_UL_FIX_SAMPLING_RATE_48K
CCFLAG += -DAIR_FIXED_RATIO_SRC
endif

##
## AIR_BLE_SWB_ENABLE
## Brief:       This option is to enable BLE SWB
## Usage:       If the value is "y",  the AIR_BLE_SWB_ENABLE option will be defined.
## Path:        driver/chip/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_BLE_SWB_ENABLE),y)
CCFLAG += -DAIR_BLE_SWB_ENABLE
CCFLAG += -DAIR_FIXED_RATIO_SRC
endif

##
## AIR_FIXED_RATIO_SRC
## Brief:       This option is to enable fixed ratio SRC.
## Usage:       If the value is "y",  the AIR_FIXED_RATIO_SRC option will be defined.
## Path:        driver/chip/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_FIXED_RATIO_SRC),y)
CCFLAG += -DAIR_FIXED_RATIO_SRC
endif

##
## AIR_VOICE_DRC_ENABLE
## Brief:       This option is to enable voice drc.
## Usage:       If the value is "y",  the AIR_VOICE_DRC_ENABLE option will be defined.
## Path:        middleware/MTK/dspfw/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_VOICE_DRC_ENABLE),y)
CCFLAG += -DAIR_VOICE_DRC_ENABLE
endif

##
## AIR_DRC_ENABLE
## Brief:       This option is to enable drc.
## Usage:       If the value is "y",  the AIR_DRC_ENABLE option will be defined.
## Path:        middleware/MTK/dspfw/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_DRC_ENABLE),y)
CCFLAG += -DAIR_DRC_ENABLE
endif

###############################################################################
##
## The following makefile options are not configurable or only for internal user. They may be removed in the future.

##
## MTK_CPU_NUMBER_1
## Brief:       Internal use.
##
ifeq ($(MTK_CPU_NUMBER_1),y)
CCFLAG += -DMTK_CPU_NUMBER_1
CCFLAG += -DMTK_MAX_CPU_NUMBER_2
endif

##
## MTK_WWE_ENABLE
## Brief:       Internal use.
## Notice:      MTK_WWE_ENABLE is a option to support WWE function.
##
ifeq ($(MTK_WWE_ENABLE),y)
CCFLAG += -DMTK_WWE_ENABLE
endif

##
## MTK_WWE_AMA_ENABLE
## Brief:       Internal use.
## Notice:      MTK_WWE_AMA_ENABLE is a option to support AMA WWE function.
##
ifeq ($(MTK_WWE_AMA_ENABLE),y)
CCFLAG += -DMTK_WWE_AMA_ENABLE
ifeq ($(MTK_WWE_AMA_USE_PIC),y)
CCFLAG += -DMTK_WWE_USE_PIC
CCFLAG += -DMTK_WWE_AMA_USE_PIC
endif
endif

##
## MTK_WWE_GSOUND_ENABLE
## Brief:       Internal use.
## Notice:      MTK_WWE_GSOUND_ENABLE is a option to support Gsound WWE function.
##
ifeq ($(MTK_WWE_GSOUND_ENABLE),y)
CCFLAG += -DMTK_WWE_GSOUND_ENABLE
ifeq ($(MTK_WWE_GSOUND_USE_PIC),y)
CCFLAG += -DMTK_WWE_USE_PIC
CCFLAG += -DMTK_WWE_GSOUND_USE_PIC
endif
endif

##
## MTK_SENSOR_SOURCE_ENABLE
## Brief:       Internal use.
## Notice:      MTK_SENSOR_SOURCE_ENABLE is a option to support Sensor Source.
##
ifeq ($(MTK_SENSOR_SOURCE_ENABLE),y)
CCFLAG += -DMTK_SENSOR_SOURCE_ENABLE
endif

##
## MTK_SUPPORT_HEAP_DEBUG
## Brief:       Internal use.
## MTK_SUPPORT_HEAP_DEBUG is a option to show heap status (alocatted or free),
## It's for RD internal development and debug. Default should be disabled.
##
ifeq ($(MTK_SUPPORT_HEAP_DEBUG),y)
CCFLAG += -DMTK_SUPPORT_HEAP_DEBUG
endif

MTK_SUPPORT_HEAP_DEBUG_ENABLE ?= $(MTK_SUPPORT_HEAP_DEBUG)
## MTK_SUPPORT_HEAP_DEBUG_ENABLE
## Brief:       Internal use.
## Notice:      MTK_SUPPORT_HEAP_DEBUG_ENABLE is a option to show heap status (alocatted or free), and will print debug info if any heap crash or heap use overflow, It's for RD internal development and debug. Default should be disabled.
##
ifeq ($(MTK_SUPPORT_HEAP_DEBUG_ENABLE),y)
CCFLAG  += -DMTK_SUPPORT_HEAP_DEBUG
endif
##

## MTK_HEAP_SIZE_GUARD_ENABLE
## Brief:       Internal use.
## MTK_HEAP_SIZE_GUARD_ENABLE is a option to profiling heap usage,
## It's for RD internal development and debug. Default should be disabled.
##
ifeq ($(MTK_HEAP_SIZE_GUARD_ENABLE),y)
LDFLAG  += -wrap=pvPortMalloc -wrap=vPortFree
CCFLAG  += -DMTK_HEAP_SIZE_GUARD_ENABLE
endif

##
## MTK_SWLA_ENABLE
## Brief:       Internal use.
## Brief:       This option is to enable and disable the Software Logical Analyzer service, Each event(task/isr activity) is recorded while CPU context switching, also support customization tag
## Usage:       If the value is "y", the MTK_SWLA_ENABLE compile option will be defined. You must also include the gva\kernel\service\module.mk in your Makefile before setting the option to "y".
## Path:        kernel/service
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_SWLA_ENABLE),y)
CCFLAG += -DMTK_SWLA_ENABLE
CCFLAG += -DPRODUCT_VERSION_STR=\"$(PRODUCT_VERSION)\"
endif
##

##
## MTK_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
## Brief:       Internal use.
## Notice:      MTK_AUDIO_SUPPORT_MULTIPLE_MICROPHONE is a option to to support multiple microphone.
##
ifeq ($(MTK_AUDIO_SUPPORT_MULTIPLE_MICROPHONE),y)
CCFLAG += -DHAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
CCFLAG += -DENABLE_2A2D_TEST
endif

##
## MTK_AUDIO_DUMP_BY_CONFIGTOOL
## Brief:       Internal use.
## Notice:      MTK_AUDIO_DUMP_BY_CONFIGTOOL is a option to choose that whether the dump path will be selected by Config Tool(y) or by Coding(n). Default should be Config Tool(y).
##
ifeq ($(MTK_AUDIO_DUMP_BY_CONFIGTOOL),y)
CCFLAG += -DMTK_AUDIO_DUMP_BY_CONFIGTOOL
endif

##
## MTK_BT_A2DP_MSBC_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_MSBC_USE_PIC is a option to use mSBC PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_MSBC_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_MSBC_USE_PIC
endif

##
## MTK_BT_A2DP_CVSD_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_CVSD_USE_PIC is a option to use CVSD PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_CVSD_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_CVSD_USE_PIC
endif

##
## MTK_BT_A2DP_LC3_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_LC3_USE_PIC is a option to use LC3 PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_LC3_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_LC3_USE_PIC
CCFLAG += -DAIR_LE_CALL_USE_LEGACY_PLC
endif

##
## MTK_BT_A2DP_ECNR_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_ECNR_USE_PIC is a option to use ECNR(Echo Cancellation / Noice Reduction) PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_ECNR_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_ECNR_USE_PIC
endif

##
## MTK_BT_CLK_SKEW_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_CLK_SKEW_USE_PIC is a option to use clock skew PIC. Default should be enabled.
##
ifeq ($(MTK_BT_CLK_SKEW_USE_PIC),y)
CCFLAG += -DMTK_BT_CLK_SKEW_USE_PIC
endif

##
## MTK_VOICE_AGC_ENABLE
## Brief:       Internal use.
## Notice:      MTK_VOICE_AGC_ENABLE is a option to enable Voice AGC. Default should be enabled.
##
ifeq ($(MTK_VOICE_AGC_ENABLE),y)
CCFLAG += -DMTK_VOICE_AGC_ENABLE
endif


##
## MTK_BT_AGC_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_AGC_USE_PIC is a option to use AGC PIC. If MTK_VOICE_AGC_ENABLE is enabled, this compile option will be enabled.
##
ifeq ($(MTK_BT_AGC_USE_PIC),y)
CCFLAG += -DMTK_BT_AGC_USE_PIC
endif



##
## MTK_PLC_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_PLC_USE_PIC is a option to use Packet Lost Compensation(PLC) PIC. Default should be enabled.
##
ifeq ($(MTK_PLC_USE_PIC),y)
CCFLAG += -DMTK_PLC_USE_PIC
endif

##
## MTK_BT_HFP_SPE_ALG_V2
## Brief:       Internal use.
## Notice:      MTK_BT_HFP_SPE_ALG_V2 is a option for mt2822 which use different algorithm interface (CPD, clk skew). Default should be enabled.
##
ifeq ($(MTK_BT_HFP_SPE_ALG_V2),y)
CCFLAG += -DMTK_BT_HFP_SPE_ALG_V2
endif

##
## ENABLE_HWSRC_CLKSKEW
## Brief:       Internal use.
## Notice:      ENABLE_HWSRC_CLKSKEW is a option for hwsrc clk skew. Default should not be enabled.
##
ifeq ($(ENABLE_HWSRC_CLKSKEW),y)
CCFLAG += -DENABLE_HWSRC_CLKSKEW
endif

##
## MTK_BT_HFP_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_HFP_ENABLE is a option for HFP feature. Default should be enabled.
##
ifeq ($(MTK_BT_HFP_ENABLE),y)
CCFLAG += -DMTK_BT_HFP_ENABLE
endif

##
## MTK_BT_HFP_FORWARDER_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_HFP_FORWARDER_ENABLE is a option for mt2822 HFP which have audio forwarder. Default should be enabled.
##
ifeq ($(MTK_BT_HFP_FORWARDER_ENABLE),y)
CCFLAG += -DMTK_BT_HFP_FORWARDER_ENABLE
endif


##
## MTK_DSP_AUDIO_MESSAGE_ENABLE
## Brief:       Internal use.
## Notice:      MTK_DSP_AUDIO_MESSAGE_ENABLE is a option to enable audio message ISR handler. Default must be enabled.
##
ifeq ($(MTK_DSP_AUDIO_MESSAGE_ENABLE),y)
CCFLAG += -DMTK_DSP_AUDIO_MESSAGE_ENABLE
endif

##
## MTK_CM4_PLAYBACK_ENABLE
## Brief:       Internal use.
## Notice:      MTK_CM4_PLAYBACK_ENABLE is a option to support CM4 playback function.
##
ifeq ($(MTK_CM4_PLAYBACK_ENABLE),y)
CCFLAG += -DMTK_CM4_PLAYBACK_ENABLE
endif

##
## MTK_CM4_RECORD_ENABLE
## Brief:       Internal use.
## Notice:      MTK_CM4_RECORD_ENABLE is a option to support CM4 record function.
##
ifeq ($(MTK_CM4_RECORD_ENABLE),y)
CCFLAG += -DMTK_CM4_RECORD_ENABLE
endif

ifeq ($(MTK_ANC_ENABLE),y)
CCFLAG += -DMTK_ANC_ENABLE
CCFLAG += -DHAL_AUDIO_ANC_ENABLE
endif

##
## MTK_LINEIN_PLAYBACK_ENABLE
## Brief:       Internal use.
## Notice:      MTK_LINEIN_PLAYBACK_ENABLE is a option to support LINEIN function.
##
ifeq ($(MTK_LINEIN_PLAYBACK_ENABLE),y)
CCFLAG += -DMTK_LINEIN_PLAYBACK_ENABLE
endif

##
## MTK_PEQ_ENABLE
## Brief:       Internal use.
## Notice:      MTK_PEQ_ENABLE is a option to support PEQ function.
##
ifeq ($(MTK_PEQ_ENABLE),y)
CCFLAG += -DMTK_PEQ_ENABLE
endif

##
## MTK_LINEIN_PEQ_ENABLE
## Brief:       Internal use.
## Notice:      MTK_LINEIN_PEQ_ENABLE is a option to support LINEIN PEQ function.
##
ifeq ($(MTK_LINEIN_PEQ_ENABLE),y)
CCFLAG += -DMTK_LINEIN_PEQ_ENABLE
endif

##
## MTK_LINEIN_INS_ENABLE
## Brief:       Internal use.
## Notice:      MTK_LINEIN_INS_ENABLE is a option to support LINEIN INS function.
##
ifeq ($(MTK_LINEIN_INS_ENABLE),y)
CCFLAG += -DMTK_LINEIN_INS_ENABLE
endif

##
## MTK_AUDIO_LOOPBACK_TEST_ENABLE
## Brief:       Internal use.
## Notice:      MTK_AUDIO_LOOPBACK_TEST_ENABLE is a option to support AUDIO LOOPBACK TEST function.
##
ifeq ($(MTK_AUDIO_LOOPBACK_TEST_ENABLE),y)
CCFLAG += -DMTK_AUDIO_LOOPBACK_TEST_ENABLE
endif

##
## AIR_SIDETONE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_SIDETONE_ENABLE is a option to support sidetone function.
##
ifeq ($(AIR_SIDETONE_ENABLE),y)
CCFLAG += -DAIR_SIDETONE_ENABLE
endif

##
## AIR_AUDIO_LDO_OUTPUT_0_85V_ENABLE
## Brief:       Internal use.
## Notice:      AIR_AUDIO_LDO_OUTPUT_0_85V_ENABLE is a option to use LDO output 0.85V.
##
ifeq ($(AIR_AUDIO_LDO_OUTPUT_0_85V_ENABLE),y)
CCFLAG += -DAIR_AUDIO_LDO_OUTPUT_0_85V_ENABLE
endif

##
## MTK_BT_A2DP_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_ENABLE is a option to support AUDIO LOOPBACK TEST function.
##
ifeq ($(MTK_BT_A2DP_ENABLE),y)
CCFLAG += -DMTK_BT_A2DP_ENABLE
endif

##
## MTK_BT_A2DP_AAC_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_AAC_ENABLE is a option to use AAC codec.
##
ifeq ($(MTK_BT_A2DP_AAC_ENABLE),y)
CCFLAG += -DMTK_BT_A2DP_AAC_ENABLE
endif

##
## MTK_BT_A2DP_AAC_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_AAC_USE_PIC is a option to use AAC PIC. If MTK_BT_A2DP_AAC_ENABLE is enabled, this one should be enabled too.
##
ifeq ($(MTK_BT_A2DP_AAC_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_AAC_USE_PIC
endif

##
## MTK_BT_A2DP_SBC_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_SBC_ENABLE is a option to use SBC codec.
##
ifeq ($(MTK_BT_A2DP_SBC_ENABLE),y)
CCFLAG += -DMTK_BT_A2DP_SBC_ENABLE
endif

##
## MTK_BT_A2DP_SBC_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_SBC_USE_PIC is a option to use SBC PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_SBC_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_SBC_USE_PIC
endif

##
## MTK_CELT_DEC_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_CELT_ENABLE is a option to support CELT decoder function.
##
ifeq ($(MTK_BT_A2DP_AIRO_CELT_ENABLE),y)
CCFLAG += -DMTK_BT_A2DP_AIRO_CELT_ENABLE
CCFLAG += -DMTK_CELT_DEC_ENABLE
ifeq ($(MTK_BT_CELT_USE_PIC),y)
CCFLAG += -DMTK_BT_CELT_USE_PIC
endif
endif

##
## MTK_BT_A2DP_CPD_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_CPD_USE_PIC is a option to use CPD PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_CPD_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_CPD_USE_PIC
endif

##
## MTK_BT_PEQ_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_PEQ_USE_PIC is a option to use PEQ PIC. If MTK_PEQ_ENABLE is enabled, this one should be enabled too.
##
ifeq ($(MTK_BT_PEQ_USE_PIC),y)
CCFLAG += -DMTK_BT_PEQ_USE_PIC
endif

##
## MTK_BT_A2DP_VENDOR_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_VENDOR_ENABLE is an option to use Vendor codec & Vendor codec PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_VENDOR_ENABLE),y)
CCFLAG += -DMTK_BT_A2DP_VENDOR_ENABLE
##
## MTK_BT_A2DP_VENDOR_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_VENDOR_USE_PIC is an option to use Vendor codec PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_VENDOR_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_VENDOR_USE_PIC
endif
##
## MTK_BT_A2DP_VENDOR_BC_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_VENDOR_BC_ENABLE is an option to apply buffer control to the Vendor codec. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_VENDOR_BC_ENABLE),y)
CCFLAG += -DMTK_BT_A2DP_VENDOR_BC_ENABLE
endif
endif

##
## MTK_BT_A2DP_VENDOR_1_ENABLE
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_VENDOR_1_ENABLE is an option to use Vendor 1 codec & Vendor 1 codec PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_VENDOR_1_ENABLE),y)
CCFLAG += -DMTK_BT_A2DP_VENDOR_1_ENABLE
##
## MTK_BT_A2DP_VENDOR_1_USE_PIC
## Brief:       Internal use.
## Notice:      MTK_BT_A2DP_VENDOR_1_USE_PIC is an option to use Vendor 1 codec PIC. Default should be enabled.
##
ifeq ($(MTK_BT_A2DP_VENDOR_1_USE_PIC),y)
CCFLAG += -DMTK_BT_A2DP_VENDOR_1_USE_PIC
endif
endif

##
## AIR_VOICE_NR_ENABLE
## Brief:       Internal use.
## Notice:      AIR_VOICE_NR_ENABLE is a option to global turn on/off voice NR function.
##
ifeq ($(AIR_VOICE_NR_ENABLE),y)
CCFLAG += -DAIR_VOICE_NR_ENABLE
endif


##
## AIR_MUTE_SMOOTHER_ENABLE
## Brief:       Internal use.
## Notice:      AIR_VOICE_NR_ENABLE is a option to global turn on/off mute smoother function.
##
ifeq ($(AIR_MUTE_SMOOTHER_ENABLE),y)
CCFLAG += -DAIR_MUTE_SMOOTHER_ENABLE
endif

##
## AIR_VOICE_PLC_ENABLE
## Brief:       Internal use.
## Notice:      AIR_VOICE_PLC_ENABLE is a option to global turn on/off voice PLC function.
##
ifeq ($(AIR_VOICE_PLC_ENABLE),y)
CCFLAG += -DAIR_VOICE_PLC_ENABLE
endif

##
## AIR_BT_HFP_CVSD_ENABLE
## Brief:       Internal use.
## Notice:      AIR_BT_HFP_CVSD_ENABLE is a option to global turn on/off CVSD encode/decode function.
##
ifeq ($(AIR_BT_HFP_CVSD_ENABLE),y)
CCFLAG += -DAIR_BT_HFP_CVSD_ENABLE
endif

##
## AIR_BT_HFP_MSBC_ENABLE
## Brief:       Internal use.
## Notice:      AIR_BT_HFP_MSBC_ENABLE is a option to global turn on/off mSBC encode/decode function.
##
ifeq ($(AIR_BT_HFP_MSBC_ENABLE),y)
CCFLAG += -DAIR_BT_HFP_MSBC_ENABLE
endif

##
## MTK_INEAR_ENHANCEMENT
## Brief:       Internal use.
## Notice:      MTK_INEAR_ENHANCEMENT is a option to support INEAR function.
##
ifeq ($(MTK_INEAR_ENHANCEMENT),y)
CCFLAG += -DMTK_INEAR_ENHANCEMENT
endif

##
## MTK_DUALMIC_INEAR
## Brief:       Internal use.
## Notice:      MTK_DUALMIC_INEAR is a option to support 2+1NR function.
##
ifeq ($(MTK_DUALMIC_INEAR),y)
CCFLAG += -DMTK_DUALMIC_INEAR
endif

##
## MTK_3RD_PARTY_NR
## Brief:       Internal use.
## Notice:      MTK_3RD_PARTY_ECNR is a option to support 3rd party ECNR function.
##
ifeq ($(MTK_3RD_PARTY_NR),y)
CCFLAG += -DMTK_3RD_PARTY_NR
endif

##
## AIR_EC120_ENABLE
## Brief:       Internal use.
## Notice:      AIR_EC120_ENABLE is a option to support EC120 function.
##
ifeq ($(AIR_EC120_ENABLE),y)
CCFLAG += -DAIR_EC120_ENABLE
endif

##
## AIR_ECNR_PREV_PART_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ECNR_PREV_PART_ENABLE is a option to support split ECNR function.
##
ifeq ($(AIR_ECNR_PREV_PART_ENABLE),y)
CCFLAG += -DAIR_ECNR_PREV_PART_ENABLE
endif

##
## AIR_ECNR_POST_PART_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ECNR_POST_PART_ENABLE is a option to support split ECNR function.
##
ifeq ($(AIR_ECNR_POST_PART_ENABLE),y)
CCFLAG += -DAIR_ECNR_POST_PART_ENABLE
endif

##
## AIR_ECNR_PREV_PART_USE_PIC_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ECNR_PREV_PART_USE_PIC_ENABLE is a option to enable split ECNR PIC.
##
ifeq ($(AIR_ECNR_PREV_PART_USE_PIC_ENABLE),y)
CCFLAG += -DAIR_ECNR_PREV_PART_USE_PIC_ENABLE
endif

##
## AIR_ECNR_POST_PART_USE_PIC_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ECNR_POST_PART_USE_PIC_ENABLE is a option to enable split ECNR PIC.
##
ifeq ($(AIR_ECNR_POST_PART_USE_PIC_ENABLE),y)
CCFLAG += -DAIR_ECNR_POST_PART_USE_PIC_ENABLE
endif

## MTK_MULTI_MIC_STREAM_ENABLE
## Brief:       Internal use.
## Notice:      MTK_MULTI_MIC_STREAM_ENABLE is a option to concurrently use AFE source.
##
ifeq ($(MTK_MULTI_MIC_STREAM_ENABLE),y)
CCFLAG += -DMTK_MULTI_MIC_STREAM_ENABLE
endif

##
## MTK_AIRDUMP_EN
## Brief:       Internal use.
## Notice:      MTK_AIRDUMP_EN is a option to to support AirDump function.
ifeq ($(MTK_AIRDUMP_EN),y)
CCFLAG += -DMTK_AIRDUMP_EN
endif

##
## MTK_AUDIO_DUMP_BY_SPDIF_ENABLE
## Brief:       Internal use.
## Notice:      MTK_AUDIO_DUMP_BY_SPDIF_ENABLE is an option to support audio dump over SPDIF interface.
ifeq ($(MTK_AUDIO_DUMP_BY_SPDIF_ENABLE),y)
CCFLAG += -DMTK_AUDIO_DUMP_BY_SPDIF_ENABLE
endif

##
## MTK_SPECIAL_FUNCTIONS_ENABLE
## Brief:       Internal use.
## Notice:      MTK_SPECIAL_FUNCTIONS_ENABLE is a option for special functions
ifeq ($(MTK_SPECIAL_FUNCTIONS_ENABLE),y)
CCFLAG += -DMTK_SPECIAL_FUNCTIONS_ENABLE
endif

##
## MTK_ANC_SURROUND_MONITOR_ENABLE
## Brief:       Internal use.
## Notice:      MTK_ANC_SURROUND_MONITOR_ENABLE is a option to support ANC surround monitor functions.
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(MTK_ANC_SURROUND_MONITOR_ENABLE),y)
CCFLAG += -DMTK_ANC_SURROUND_MONITOR_ENABLE
endif
endif

##
## AIR_ANC_WIND_DETECTION_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ANC_WIND_DETECTION_ENABLE is a option to support wind detection to suppress noise when ANC on.
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(AIR_ANC_WIND_DETECTION_ENABLE),y)
CCFLAG += -DAIR_ANC_WIND_DETECTION_ENABLE
ifeq ($(AIR_WIND_DETECTION_USE_PIC),y)
CCFLAG += -DAIR_WIND_DETECTION_USE_PIC
endif
MTK_ANC_SURROUND_MONITOR_ENABLE = y
CCFLAG += -DMTK_ANC_SURROUND_MONITOR_ENABLE
endif
endif


##
## AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ANC_ENVIRONMENT_DETECTION_ENABLE is a option to support environment noise level detection to suppress ANC.
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE),y)
CCFLAG += -DAIR_ANC_ENVIRONMENT_DETECTION_ENABLE
ifeq ($(AIR_ANC_ENVIRONMENT_DETECTIONE_USE_PIC),y)
CCFLAG += -DAIR_ANC_ENVIRONMENT_DETECTIONE_USE_PIC
endif
MTK_ANC_SURROUND_MONITOR_ENABLE = y
CCFLAG += -DMTK_ANC_SURROUND_MONITOR_ENABLE
endif
endif

##
## AIR_ANC_USER_UNAWARE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ANC_USER_UNAWARE_ENABLE is a option to get better noise cancellation
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(AIR_ANC_USER_UNAWARE_ENABLE),y)
CCFLAG += -DAIR_ANC_USER_UNAWARE_ENABLE
CCFLAG += -DAIR_USER_UNAWARE_USE_PIC
MTK_ANC_SURROUND_MONITOR_ENABLE = y
CCFLAG += -DMTK_ANC_SURROUND_MONITOR_ENABLE
endif
endif

##
## MTK_AUDIO_TRANSMITTER_ENABLE
## Brief:       Internal use.
## Notice:      MTK_AUDIO_TRANSMITTER_ENABLE is a option to to support audio transmitter.
ifeq ($(MTK_AUDIO_TRANSMITTER_ENABLE), y)
CCFLAG += -DMTK_AUDIO_TRANSMITTER_ENABLE
endif

##
## MTK_AUDIO_BT_COMMON_ENABLE
## Brief:       Internal use.
## Notice:      MTK_AUDIO_BT_COMMON_ENABLE is a option to to support audio bt common architecture.
ifeq ($(MTK_AUDIO_BT_COMMON_ENABLE), y)
CCFLAG += -DMTK_AUDIO_BT_COMMON_ENABLE
endif

##
## MTK_GAMING_MODE_HEADSET
## Brief:       Internal use.
## Notice:      MTK_GAMING_MODE_HEADSET is a option to to enable gaming mode on headset.
ifeq ($(MTK_GAMING_MODE_HEADSET), y)
CCFLAG += -DMTK_GAMING_MODE_HEADSET
endif

##
## AIR_GAMING_MODE_HEADSET_ECNR_ENABLE
## Brief:       Internal use.
## Notice:      AIR_GAMING_MODE_HEADSET_ECNR_ENABLE is a option to to enable gaming mode on headset with ECNR.
ifeq ($(AIR_GAMING_MODE_HEADSET_ECNR_ENABLE), y)
CCFLAG += -DAIR_GAMING_MODE_HEADSET_ECNR_ENABLE
endif

##
## AIR_GAMING_MODE_DONGLE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_GAMING_MODE_DONGLE_ENABLE is a option to to enable gaming mode on dongle.
ifeq ($(AIR_GAMING_MODE_DONGLE_ENABLE), y)
CCFLAG += -DAIR_GAMING_MODE_DONGLE_ENABLE
endif

##
## AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
## Brief:       Internal use.
## Notice:      AIR_GAMING_MODE_DONGLE_ECNR_ENABLE is a option to to enable gaming mode on dongle with ECNR.
ifeq ($(AIR_GAMING_MODE_DONGLE_ECNR_ENABLE), y)
CCFLAG += -DAIR_GAMING_MODE_DONGLE_ECNR_ENABLE
endif

##
## AIR_GAMING_MODE_UPLINK_LANTENCY_DEBUG_ENABLE
## Brief:       Internal use.
## Notice:      AIR_GAMING_MODE_UPLINK_LANTENCY_DEBUG_ENABLE is a option to debug Uplink latency on gaming mode.
ifeq ($(AIR_GAMING_MODE_UPLINK_LANTENCY_DEBUG_ENABLE), y)
CCFLAG += -DAIR_GAMING_MODE_UPLINK_LANTENCY_DEBUG_ENABLE
endif

##
## AIR_CELT_ENC_ENABLE
## Brief:       Internal use.
## Notice:      AIR_CELT_ENC_ENABLE is a option to support CELT encoder function.
##
ifeq ($(AIR_CELT_ENC_ENABLE),y)
CCFLAG += -DAIR_CELT_ENC_ENABLE
endif

##
## AIR_SOFTWARE_SRC_ENABLE
## Brief:       Internal use.
## Notice:      AIR_SOFTWARE_SRC_ENABLE is a option to enable software src.
##
ifeq ($(AIR_SOFTWARE_SRC_ENABLE),y)
CCFLAG += -DAIR_SOFTWARE_SRC_ENABLE
endif

##
## AIR_SOFTWARE_CLK_SKEW_ENABLE
## Brief:       Internal use.
## Notice:      AIR_SOFTWARE_CLK_SKEW_ENABLE is a option to enable software clk skew.
##
ifeq ($(AIR_SOFTWARE_CLK_SKEW_ENABLE),y)
CCFLAG += -DAIR_SOFTWARE_CLK_SKEW_ENABLE
endif

##
## AIR_SOFTWARE_GAIN_ENABLE
## Brief:       Internal use.
## Notice:      AIR_SOFTWARE_GAIN_ENABLE is a option to enable software gain.
##
ifeq ($(AIR_SOFTWARE_GAIN_ENABLE),y)
CCFLAG += -DAIR_SOFTWARE_GAIN_ENABLE
endif

##
## AIR_SOFTWARE_MIXER_ENABLE
## Brief:       Internal use.
## Notice:      AIR_SOFTWARE_MIXER_ENABLE is a option to enable software mixer.
##
ifeq ($(AIR_SOFTWARE_MIXER_ENABLE),y)
CCFLAG += -DAIR_SOFTWARE_MIXER_ENABLE
endif

##
## AIR_SOFTWARE_BUFFER_ENABLE
## Brief:       Internal use.
## Notice:      AIR_SOFTWARE_BUFFER_ENABLE is a option to enable software buffer.
##
ifeq ($(AIR_SOFTWARE_BUFFER_ENABLE),y)
CCFLAG += -DAIR_SOFTWARE_BUFFER_ENABLE
endif

##
## ENABLE_SIDETONE_RAMP_TIMER
## Brief:       Internal use.
## Notice:      ENABLE_SIDETONE_RAMP_TIMER is a option to to support Sidetone gain ramp for depop.
##
ifeq ($(ENABLE_SIDETONE_RAMP_TIMER),y)
CCFLAG += -DENABLE_SIDETONE_RAMP_TIMER
endif

##
## MTK_USER_TRIGGER_FF_ENABLE
## Brief:       Internal use.
## Notice:      MTK_USER_TRIGGER_FF_ENABLE is a option to to support User triggered adaptive ANC
##
ifeq ($(MTK_USER_TRIGGER_FF_ENABLE),y)
CCFLAG += -DMTK_USER_TRIGGER_FF_ENABLE
CCFLAG += -DMTK_USER_TRIGGER_ADAPTIVE_FF_V2
endif

##
## AIR_AUDIO_DETACHABLE_MIC_ENABLE
## Brief:       Internal use.
## Notice:      AIR_AUDIO_DETACHABLE_MIC_ENABLE is a option to enable detachable mic control.
##
ifeq ($(AIR_AUDIO_DETACHABLE_MIC_ENABLE),y)
CCFLAG += -DAIR_AUDIO_DETACHABLE_MIC_ENABLE
endif

##
## AIR_IGO_NR_ENABLE
## Brief:       Internal use.
## Notice:      AIR_IGO_NR_ENABLE is a option to enable detachable mic control.
##
ifeq ($(AIR_IGO_NR_ENABLE),y)
CCFLAG += -DAIR_IGO_NR_ENABLE
endif

##
## AIR_IGO_NR_PLUS_1_ENABLE
## Brief:       Internal use.
## Notice:      AIR_IGO_NR_PLUS_1_ENABLE is a option to enable detachable mic control.
##
ifeq ($(AIR_IGO_NR_PLUS_1_ENABLE),y)
CCFLAG += -DAIR_IGO_NR_PLUS_1_ENABLE
endif

##
## AIR_ULL_VOICE_LOW_LATENCY_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ULL_VOICE_LOW_LATENCY_ENABLE is a option to enable ULL voice low latency mode.
##
ifeq ($(AIR_ULL_VOICE_LOW_LATENCY_ENABLE),y)
CCFLAG += -DAIR_ULL_VOICE_LOW_LATENCY_ENABLE
endif

##
## AIR_ADVANCED_PASSTHROUGH_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ADVANCED_PASSTHROUGH_ENABLE is a option to to enable advanced_passthrough feature.
ifeq ($(AIR_ADVANCED_PASSTHROUGH_ENABLE), y)
CCFLAG += -DAIR_ADVANCED_PASSTHROUGH_ENABLE
AIR_FIXED_RATIO_SRC = y
CCFLAG += -DAIR_FIXED_RATIO_SRC
AIR_SOFTWARE_GAIN_ENABLE = y
CCFLAG += -DAIR_SOFTWARE_GAIN_ENABLE

AIR_AFC_LIB = $(strip $(ROOTDIR))/prebuilt/middleware/MTK/dspalg/afc/ab156x/libafc_protected.a
ifeq ($(AIR_AFC_LIB), $(wildcard $(AIR_AFC_LIB)))
    AIR_AFC_ENABLE = y
    CCFLAG += -DAIR_AFC_ENABLE
    AIR_AFC_CODE = $(strip $(ROOTDIR))/middleware/MTK/dspalg/afc_protected/module.mk
    ifeq ($(AIR_AFC_CODE), $(wildcard $(AIR_AFC_CODE)))
        AIR_AFC_LIB_ENABLE = n
    else
        AIR_AFC_LIB_ENABLE = y
    endif
    AIR_AFC_USE_PIC = y
    ifeq ($(AIR_AFC_USE_PIC),y)
        CCFLAG += -DAIR_AFC_USE_PIC
    endif
endif
AIR_LD_NR_LIB = $(strip $(ROOTDIR))/prebuilt/middleware/MTK/dspalg/ld_nr/ab156x/libld_nr_protected.a
ifeq ($(AIR_LD_NR_LIB), $(wildcard $(AIR_LD_NR_LIB)))
    AIR_LD_NR_ENABLE = y
    CCFLAG += -DAIR_LD_NR_ENABLE
    AIR_LD_NR_CODE = $(strip $(ROOTDIR))/middleware/MTK/dspalg/ld_nr_protected/module.mk
    ifeq ($(AIR_LD_NR_CODE), $(wildcard $(AIR_LD_NR_CODE)))
        AIR_LD_NR_LIB_ENABLE = n
    else
        AIR_LD_NR_LIB_ENABLE = y
    endif
    AIR_LD_NR_USE_PIC = y
    ifeq ($(AIR_LD_NR_USE_PIC),y)
        CCFLAG += -DAIR_LD_NR_USE_PIC
    endif
endif
AIR_AT_AGC_LIB = $(strip $(ROOTDIR))/prebuilt/middleware/MTK/dspalg/at_agc/ab156x/libat_agc_protected.a
ifeq ($(AIR_AT_AGC_LIB), $(wildcard $(AIR_AT_AGC_LIB)))
    AIR_AT_AGC_ENABLE = y
    CCFLAG += -DAIR_AT_AGC_ENABLE
    AIR_AT_AGC_CODE = $(strip $(ROOTDIR))/middleware/MTK/dspalg/at_agc_protected/module.mk
    ifeq ($(AIR_AT_AGC_CODE), $(wildcard $(AIR_AT_AGC_CODE)))
        AIR_AT_AGC_LIB_ENABLE = n
    else
        AIR_AT_AGC_LIB_ENABLE = y
    endif
    AIR_AT_AGC_USE_PIC = y
    ifeq ($(AIR_AT_AGC_USE_PIC),y)
        CCFLAG += -DAIR_AT_AGC_USE_PIC
    endif
endif
AIR_UL_FIX_SAMPLING_RATE_48K = y
CCFLAG += -DAIR_UL_FIX_SAMPLING_RATE_48K
endif

##
## AIR_BLE_AUDIO_DONGLE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_BLE_AUDIO_DONGLE_ENABLE is a option to to enable gaming mode on dongle.
ifeq ($(AIR_BLE_AUDIO_DONGLE_ENABLE), y)
CCFLAG += -DAIR_BLE_AUDIO_DONGLE_ENABLE
endif

##
## AIR_BLE_AUDIO_SOURCE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_BLE_AUDIO_DONGLE_ENABLE is a option to to enable gaming mode on dongle.
ifeq ($(AIR_BLE_AUDIO_SOURCE_ENABLE), y)
CCFLAG += -DAIR_BLE_AUDIO_SOURCE_ENABLE
endif

##
## AIR_HWSRC_TX_TRACKING_ENABLE
## Brief:       Internal use.
## Notice:      AIR_HWSRC_TX_TRACKING_ENABLE is a option to enable i2s slave tx hwsrc tracking mode.
##
ifeq ($(AIR_HWSRC_TX_TRACKING_ENABLE),y)
CCFLAG += -DAIR_HWSRC_TX_TRACKING_ENABLE
endif

##
## AIR_HWSRC_RX_TRACKING_ENABLE
## Brief:       Internal use.
## Notice:      AIR_HWSRC_RX_TRACKING_ENABLE is a option to enable i2s slave rx hwsrc tracking mode.
##
ifeq ($(AIR_HWSRC_RX_TRACKING_ENABLE),y)
CCFLAG += -DAIR_HWSRC_RX_TRACKING_ENABLE
endif

##
## AIR_DUAL_CHIP_I2S_ENABLE
## Brief:       Internal use.
## Notice:      AIR_DUAL_CHIP_I2S_ENABLE is a option to enable i2s of dual chip.
##
ifeq ($(AIR_DUAL_CHIP_I2S_ENABLE),y)
CCFLAG += -DAIR_DUAL_CHIP_I2S_ENABLE
endif

##
## AIR_DONGLE_AFE_USAGE_CHECK_ENABLE
## Brief:       Internal use.
## Notice:      AIR_DONGLE_AFE_USAGE_CHECK_ENABLE is a option to check if AFE module been used, default should be disabled for non-dongle project.
##
ifeq ($(AIR_DONGLE_AFE_USAGE_CHECK_ENABLE),y)
CCFLAG += -DAIR_DONGLE_AFE_USAGE_CHECK_ENABLE
endif

##
## AIR_VOLUME_ESTIMATOR_ENABLE
## Brief:       Internal use.
## Notice:      AIR_VOLUME_ESTIMATOR_ENABLE is a option to to enable volume estimator.
ifeq ($(AIR_VOLUME_ESTIMATOR_ENABLE), y)
CCFLAG += -DAIR_VOLUME_ESTIMATOR_ENABLE
endif

##
## AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE is a option to to enable game/chat volume smart balance feature.
ifeq ($(AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE), y)
CCFLAG += -DAIR_VOLUME_ESTIMATOR_ENABLE
CCFLAG += -DAIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
endif

##
## AIR_AUDIO_HARDWARE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_AUDIO_HARDWARE_ENABLE is a option to to enable audio hardware.
ifeq ($(AIR_AUDIO_HARDWARE_ENABLE), y)
CCFLAG += -DAIR_AUDIO_HARDWARE_ENABLE
endif

##
## AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE
## Brief:       Internal use.
## Notice:      AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE is a option to to enable dual chip master hwsrc tracking mode feature.
ifeq ($(AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE), y)
CCFLAG += -DAIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE
endif
##
## AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE
## Brief:       Internal use.
## Notice:      AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE is a option to to enable A2DP drc use dgain.
ifeq ($(AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE), y)
CCFLAG += -DAIR_A2DP_DRC_TO_USE_DGAIN_ENABLE
endif

##
## AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ_ENABLE
## Brief:       Internal use.
## Usage:       If the value is "n",  the AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ option will not be defined.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ_ENABLE),y)
CCFLAG += -DAIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ
endif

## AIR_FIXED_SUB_DL_HIGH_RES_ENABLE
## Brief:       Internal use.
## Usage:       If the value is "n",  the AIR_FIXED_SUB_DL_HIGH_RES_ENABLE option will not be defined.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE),y)
CCFLAG += -DAIR_FIXED_SUB_DL_HIGH_RES_ENABLE
endif

##
## AIR_AUDIO_MIXER_GAIN_ENABLE
## Brief:       Internal use.
## Usage:       If the value is "n",  the AIR_AUDIO_MIXER_GAIN_ENABLE option will not be defined.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_AUDIO_MIXER_GAIN_ENABLE),y)
CCFLAG += -DAIR_AUDIO_MIXER_GAIN_ENABLE
endif
###############################################################################


ifneq ($(MTK_LOWPOWER_LEVEL),)
CCFLAG += -DMTK_LOWPOWER_LEVEL=$(MTK_LOWPOWER_LEVEL)
endif

ifeq ($(ENABLE_HWSRC_ON_MAIN_STREAM),y)
CCFLAG += -DENABLE_HWSRC_ON_MAIN_STREAM
endif

ifeq ($(MTK_HWSRC_IN_STREAM),y)
CCFLAG += -DMTK_HWSRC_IN_STREAM
endif

ifeq ($(AB1568_BRING_UP_DSP_DEFAULT_HW_LOOPBACK),y)
CCFLAG += -DAB1568_BRING_UP_DSP_DEFAULT_HW_LOOPBACK
endif

ifeq ($(MTK_BT_AVM_SHARE_BUF),y)
CCFLAG += -DMTK_BT_AVM_SHARE_BUF
endif

ifeq ($(MTK_AUDIO_PLC_ENABLE),y)
CCFLAG += -DMTK_AUDIO_PLC_ENABLE
endif

ifeq ($(MTK_SLT_AUDIO_HW),y)
CCFLAG += -DMTK_SLT_AUDIO_HW
endif

ifeq ($(AIR_WIRED_AUDIO_ENABLE),y)
CCFLAG += -DAIR_WIRED_AUDIO_ENABLE
endif

ifeq ($(AIR_AUDIO_HW_LOOPBACK_ENABLE),y)
CCFLAG += -DAIR_AUDIO_HW_LOOPBACK_ENABLE
endif

ifeq ($(AIR_HFP_DNN_PATH_ENABLE),y)
CCFLAG += -DAIR_HFP_DNN_PATH_ENABLE
endif

ifeq ($(AIR_DNN_LIB_ENABLE),y)
CCFLAG += -DAIR_DNN_LIB_ENABLE
endif

##
## AIR_SILENCE_DETECTION_ENABLE
## Brief:       Internal use.
## Notice:      AIR_SILENCE_DETECTION_ENABLE is a option to enabled silence detection feature.
##
ifeq ($(AIR_SILENCE_DETECTION_ENABLE),y)
CCFLAG += -DAIR_SILENCE_DETECTION_ENABLE
endif

ifeq ($(AIR_A2DP_SYNC_CONTROL_ENABLE),y)
CCFLAG += -DAIR_A2DP_SYNC_START_ENABLE
CCFLAG += -DAIR_A2DP_SYNC_STOP_ENABLE
endif

ifeq ($(AIR_HFP_SYNC_CONTROL_ENABLE),y)
CCFLAG += -DAIR_HFP_SYNC_START_ENABLE
CCFLAG += -DAIR_HFP_SYNC_STOP_ENABLE
endif

##
## AIR_BT_CLK_SKEW_ENABLE
## Brief:       Internal use.
## Notice:      AIR_BT_CLK_SKEW_ENABLE is a option to enabled clk skew feature.
##
ifneq ($(AIR_BT_CLK_SKEW_ENABLE),n)
CCFLAG += -DAIR_BT_CLK_SKEW_ENABLE
endif

##
## AIR_A2DP_NLE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_A2DP_NLE_ENABLE is a option to enabled A2DP NLE feature.
##
ifeq ($(AIR_A2DP_NLE_ENABLE),y)
CCFLAG += -DAIR_A2DP_NLE_ENABLE
endif

##
## AIR_ULL_DONGLE_LINE_IN_OUT_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ULL_DONGLE_LINE_IN_OUT_ENABLE is a option to enabled Line in/out in ULL dongle.
##
ifeq ($(AIR_ULL_DONGLE_LINE_IN_OUT_ENABLE),y)
CCFLAG += -DAIR_ULL_DONGLE_LINE_IN_OUT_ENABLE
endif

##
## AIR_DUAL_CHIP_NR_ON_MASTER_ENABLE
## Brief:       Internal use.
## Notice:      AIR_DUAL_CHIP_NR_ON_MASTER_ENABLE is a option to enabled MASTER NR.
##
ifeq ($(AIR_DUAL_CHIP_NR_ON_MASTER_ENABLE),y)
CCFLAG += -DAIR_DUAL_CHIP_NR_ON_MASTER_ENABLE
endif