PRODUCT_VERSION                       = 1565
MTK_SYSLOG_VERSION_2                  ?= y
MTK_SYSLOG_SUB_FEATURE_STRING_LOG_SUPPORT = y
MTK_SYSLOG_SUB_FEATURE_BINARY_LOG_SUPPORT = y
MTK_SYSLOG_SUB_FEATURE_OFFLINE_DUMP_ACTIVE_MODE = y
AIR_ATCI_VIA_PORT_SERVICE_ENABLE      = n
MTK_NO_PSRAM_ENABLE                   = y
AIR_ATCI_VIA_MUX_ENABLE               = y
MTK_DEBUG_PLAIN_LOG_ENABLE            ?= n
MTK_CPU_NUMBER_0                      ?= y
MTK_MUX_ENABLE                        ?= y
FPGA_ENV                              ?= n
MTK_AUDIO_LOOPBACK_TEST_ENABLE ?= y
MTK_AUDIO_TRANSMITTER_ENABLE          ?= y
MTK_AVM_DIRECT                        ?= y
MTK_BT_CODEC_ENABLED                  ?= y
MTK_BT_CM_SUPPORT                     ?= y
AIR_BT_AUDIO_SYNC_ENABLE              = y
AIR_AUDIO_MP3_ENABLE ?= $(MTK_AUDIO_MP3_ENABLED)
AIR_MP3_DECODER_ENABLE ?= $(MTK_MP3_DECODER_ENABLED)
AIR_MP3_CODEC_TASK_DEDICATE_ENABLE ?= $(MTK_MP3_CODEC_TASK_DEDICATE)
AIR_MP3_STEREO_SUPPORT_ENABLE ?= $(MTK_MP3_STEREO_SUPPORT)
AIR_3RD_PARTY_NR_ENABLE ?= $(MTK_3RD_PARTY_NR)
AIR_DUALMIC_INEAR_ENABLE ?= $(MTK_DUALMIC_INEAR_EN)
AIR_INEAR_ENHANCEMENT_ENABLE ?= $(MTK_INEAR_ENHANCEMENT_EN)
AIR_AIRDUMP_ENABLE ?= $(MTK_AIRDUMP_EN)
AIR_DC_COMPENSATION_PARA_ENABLE ?= $(DC_COMPENSATION_PARA_ENABLE)
AIR_AUDIO_SUPPORT_MULTIPLE_MICROPHONE_ENABLE ?= $(MTK_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
AIR_SMT_SPK_TO_MIC_TEST_ENABLE ?= $(MTK_SMT_AUDIO_TEST)
AIR_PEQ_ENABLE ?= $(MTK_PEQ_ENABLE)
AIR_LINEIN_PEQ_ENABLE ?= $(MTK_LINEIN_PEQ_ENABLE)
ENABLE_HWSRC_CLKSKEW                  ?= y

# BT Relate
MTK_BT_DUO_ENABLE                   ?= y
MTK_BT_ENABLE                       ?= y
MTK_BLE_ONLY_ENABLE                 ?= n
MTK_BT_HFP_ENABLE                   ?= y
MTK_BT_HSP_ENABLE                   ?= y
MTK_BT_AVRCP_ENABLE                 ?= y
MTK_BT_AVRCP_ENH_ENABLE             ?= y
MTK_BT_A2DP_ENABLE                  ?= y
MTK_BT_PBAP_ENABLE                  ?= n
MTK_BT_SPP_ENABLE                   ?= y
MTK_BT_TIMER_EXTERNAL_ENABLE        ?= y
MTK_PORT_SERVICE_ENABLE             ?= y
MTK_PORT_SERVICE_BT_ENABLE          ?= y
MTK_MUX_BT_ENABLE                   ?= y


ifeq ($(IC_CONFIG),ab156x)
COM_CFLAGS += -DAB156X
endif
ifeq ($(IC_TYPE),ab1565)
COM_CFLAGS += -DAB1565
endif
ifeq ($(IC_TYPE),ab1568)
COM_CFLAGS += -DAB1568
endif

AR      = $(BINPATH)/arm-none-eabi-ar
CC      = $(BINPATH)/arm-none-eabi-gcc
CXX     = $(BINPATH)/arm-none-eabi-g++
OBJCOPY = $(BINPATH)/arm-none-eabi-objcopy
SIZE    = $(BINPATH)/arm-none-eabi-size
OBJDUMP = $(BINPATH)/arm-none-eabi-objdump


ALLFLAGS = -mlittle-endian -mthumb -mcpu=cortex-m4
FPUFLAGS = -Wdouble-promotion -mfpu=fpv4-sp-d16 -mfloat-abi=hard
COM_CFLAGS += $(ALLFLAGS) $(FPUFLAGS) -ffunction-sections -fdata-sections -fno-builtin -Wimplicit-function-declaration
COM_CFLAGS += -Os -Wall -fno-strict-aliasing -fno-common
ifneq ($(MTK_SOURCE_COMPILE_DEBUG_INFO_ENABLE),n)
   COM_CFLAGS += -gdwarf-2
   endif
COM_CFLAGS += -Wall -Wimplicit-function-declaration -Werror=uninitialized -Wno-error=maybe-uninitialized -Werror=return-type
COM_CFLAGS += -DPCFG_OS=2 -D_REENT_SMALL -Wno-error -Wno-switch
COM_CFLAGS += -DPRODUCT_VERSION=$(PRODUCT_VERSION)
COM_CFLAGS += -DCORE_CM4
COM_CFLAGS += -D$(TARGET)_BOOTING
ifeq ($(FPGA_ENV),y)
COM_CFLAGS += -DFPGA_ENV
endif
LDFLAGS += -Wl,--gc-sections -Wl,-wrap=printf
LDFLAGS += -u _printf_float
LDFLAGS += -Wl,-wrap=memcpy -Wl,-wrap=memset -Wl,-wrap=memclr

##
## AIR_BT_AUDIO_SYNC_ENABLE
## Brief:       This option is to enable BT-Audio Sync in DSP side.
## Usage:       Enable the feature by configuring it as y.
## Path:        None
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_BT_AUDIO_SYNC_ENABLE),y)
CFLAGS += -DAIR_BT_AUDIO_SYNC_ENABLE
endif

##
## MTK_MEMORY_MONITOR_ENABLE
## Brief:       This option is to debug memory corruption.
## Usage:       The valid values are y,n.
##              The setting will determine if memory monitor will be used.
##              However, the setting has no effect on the prebuilt library.
## Path:        kernel/service/memory_monitor
## Dependency:  None
## Notice:      None
##
ifeq ($(MTK_MEMORY_MONITOR_ENABLE),y)
# COM_CFLAGS += -fstack-protector-all
COM_CFLAGS += -DMTK_MEMORY_MONITOR_ENABLE
endif

##
## AIR_SYSTEM_HANG_TRACER_ENABLE
## Brief:       This option is to debug system hang issue.
## Usage:       The valid values are y, o1, o2.
##              The setting will determine which level of system hang tracer will be used.
##              However, the setting has no effect on the prebuilt library.
##              y    : wdt interrupt mode will be used in tracing system hang.
##              o1   : wdt interrupt mode will be used in tracing system hang.
##              o2   : wdt reset mode and rtc module will be used in tracing system hang.
## Path:        kernel/service/systmehang_tracer
## Dependency:  None
## Notice:      None
##

AIR_SYSTEM_HANG_TRACER_ENABLE ?= $(MTK_SYSTEM_HANG_TRACER_ENABLE)
ifeq ($(AIR_SYSTEM_HANG_TRACER_ENABLE),y)
COM_CFLAGS += -DMTK_SYSTEM_HANG_TRACER_ENABLE
endif

ifeq ($(AIR_SYSTEM_HANG_TRACER_ENABLE),o1)
COM_CFLAGS += -DMTK_SYSTEM_HANG_TRACER_ENABLE
COM_CFLAGS += -DMTK_SYSTEM_HANG_TRACER_ENABLE_O1
endif

ifeq ($(AIR_SYSTEM_HANG_TRACER_ENABLE),o2)
COM_CFLAGS += -DMTK_SYSTEM_HANG_TRACER_ENABLE
COM_CFLAGS += -DMTK_SYSTEM_HANG_TRACER_ENABLE_O2
endif

ifeq ($(AIR_SYSTEM_HANG_TRACER_ENABLE),mp)
COM_CFLAGS += -DMTK_SYSTEM_HANG_TRACER_ENABLE
COM_CFLAGS += -DMTK_SYSTEM_HANG_TRACER_ENABLE_MP
endif

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
COM_CFLAGS += -DMTK_DEBUG_PLAIN_LOG_ENABLE
endif

##
## AIR_SYSLOG_BUFFER_EXPAND_ENABLE
## Brief:       This option is used to increase syslog buffer.
## Usage:       Enable the feature by configuring it as y.
##              y : increase syslog buffer
##              n : slim syslog buffer
## Path:        kernel/service/syslog
## Dependency:  None
## Notice:      None
## Related doc :None
##
ifeq ($(AIR_SYSLOG_BUFFER_EXPAND_ENABLE),y)
COM_CFLAGS += -DAIR_SYSLOG_BUFFER_EXPAND_ENABLE
endif

##
## AIR_BOOTREASON_CHECK_ENABLE
## Brief:       This option is to check boot reason and get the information about the last crash.
## Usage:       The valid values are n, y.
##              n   : no effect
##              y   : enable boot reason check feature.
## Path:        kernel/service/bootreason_check
## Dependency:  None
## Notice:      None
##
AIR_BOOTREASON_CHECK_ENABLE ?= $(MTK_BOOTREASON_CHECK_ENABLE)
ifeq ($(AIR_BOOTREASON_CHECK_ENABLE),y)
COM_CFLAGS += -DMTK_BOOTREASON_CHECK_ENABLE
endif


##
## AIR_DEBUG_LEVEL
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
## Realted doc: Please refer to doc/Airoha_IoT_SDK_for_BT_Audio_155x_API_Reference_Manual.html
##
AIR_DEBUG_LEVEL  ?=  $(MTK_DEBUG_LEVEL)
ifeq ($(AIR_DEBUG_LEVEL),)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_DEBUG
COM_CFLAGS += -DMTK_DEBUG_LEVEL_INFO
COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(AIR_DEBUG_LEVEL),error)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(AIR_DEBUG_LEVEL),warning)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(AIR_DEBUG_LEVEL),info)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_INFO
COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(AIR_DEBUG_LEVEL),debug)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_DEBUG
COM_CFLAGS += -DMTK_DEBUG_LEVEL_INFO
COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(AIR_DEBUG_LEVEL),printf)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_PRINTF
endif

ifeq ($(AIR_DEBUG_LEVEL),none)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_NONE
endif

##
## MTK_SAVE_LOG_TO_FLASH_ENABLE
## Brief:       This option is used to enable log saving to flash feature.
## Usage:       Enable the feature by configuring it as y.
##              y : save runtime log content to flash
##              n : transfer runtime log over logging port
## Path:        kernel/service/syslog
## Dependency:  flash driver
## Notice:      Reserve flash blocks to store runtime log
## Related doc :None
##
ifeq ($(MTK_SAVE_LOG_TO_FLASH_ENABLE),y)
    CFLAGS += -DMTK_SAVE_LOG_TO_FLASH_ENABLE
endif
##

##
## AIR_MINIDUMP_ENABLE
## Brief:       This option is used to enable minidump feature.
## Usage:       Enable the feature by configuring it as y.
##              y : minidump
##              n : fulldump
## Path:        kernel/service/exception
## Dependency:  flash driver
## Notice:      Reserve flash blocks to store runtime log
## Related doc :None
##

AIR_MINIDUMP_ENABLE ?= $(MTK_MINIDUMP_ENABLE)
ifeq ($(AIR_MINIDUMP_ENABLE),y)
    COM_CFLAGS += -DMTK_MINIDUMP_ENABLE
endif
##

##
## AIR_FULLDUMP_ENABLE
## Brief:       This option is used to enable fulldump feature.
## Usage:       Enable the feature by configuring it as y.
##              y : fulldump enable
##              n : fulldump disable
## Path:        kernel/service/exception
## Dependency:  syslog
## Notice:      Output dump information by UART or USB
## Related doc :None
##
AIR_FULLDUMP_ENABLE ?= $(MTK_FULLDUMP_ENABLE)
ifeq ($(AIR_FULLDUMP_ENABLE),y)
    COM_CFLAGS += -DMTK_FULLDUMP_ENABLE
endif
##

##
## MTK_SPI_EXTERNAL_SERIAL_FLASH_ENABLED
## Brief:       This option is to enable external flash feature.
## Usage:       Enable the feature by configuring it as y.
##              y : enable external flash feature.
##              n   : no effect
## Path:        driver/board/ab155x_evk/bsp_external_flash
## Dependency:  None
## Notice:      None
## Related doc :None
##
ifeq ($(MTK_SPI_EXTERNAL_SERIAL_FLASH_ENABLED),y)
    CFLAGS += -DMTK_SPI_EXTERNAL_SERIAL_FLASH_ENABLED
endif
##

##
## AIR_SYSTEM_HANG_CHECK_ENABLE
## Brief:       This option is to enable system hang issue debug feature .
## Usage:       Enable the feature by configuring it as y.
## Path:        kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F       
## Dependency:  None
## Notice:      None
## Relative doc:None
##
AIR_SYSTEM_HANG_CHECK_ENABLE ?= $(MTK_SYSTEM_HANG_CHECK_ENABLE)
ifeq ($(AIR_SYSTEM_HANG_CHECK_ENABLE),y)
COM_CFLAGS         += -DMTK_SYSTEM_HANG_CHECK_ENABLE
endif


##
## MTK_NO_PSRAM_ENABLE
## Brief:       This option is to enable/disable PSRAM.
## Usage:       If the value is "y", the MTK_HAL_PLAIN_LOG_ENABLE will be defined, PSRAM will be initialized 
##              in the initialization phase the option to "y" include the driver/chip/mtxxxx/module.mk.
## Path:        driver/chip/mtxxxx       
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_NO_PSRAM_ENABLE),y)
CFLAGS         += -DMTK_NO_PSRAM_ENABLE
endif

##
## MTK_PROMPT_SOUND_ENABLE
## Brief:       This option is to enable prompt sound feature.
## Usage:       If the value is "y",  the MTK_PROMPT_SOUND_ENABLE and MTK_AUDIO_MIXER_SUPPORT compile options will be defined.
## Path:        middleware/MTK/audio/audio_mixer/
## Dependency:  Must enable HAL_AUDIO_MODULE_ENABLED feature.mk under the project GCC folder..
## Notice:      None.
## Relative doc:None
##
ifeq ($(MTK_PROMPT_SOUND_ENABLE), y)
  CFLAGS += -DMTK_PROMPT_SOUND_ENABLE
  CFLAGS += -DMTK_AUDIO_MIXER_SUPPORT
endif

##
## MTK_SYSTEM_AT_COMMAND_ENABLE
## Brief:       This option is to enable and disable at commands for system service, include show task information, heap infomation, and force system crash to get memory dump
## Usage:       Please use AT+SYSTEM=? to get detail info for this atcmd
## Path:        mcu\middleware\MTK\atci\at_command\at_command_system.c
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_SYSTEM_AT_COMMAND_ENABLE),y)
COM_CFLAGS += -DMTK_SYSTEM_AT_COMMAND_ENABLE
endif

##
## MTK_OS_CPU_UTILIZATION_ENABLE
## Brief:       This option is to enable and disable cpu utilization function.
## Usage:       Please use AT+UTILIZATION=? to get detail info for this atcmd
## Path:        mcu\middleware\MTK\atci\at_command\at_command_system.c
## Dependency:  None
## Notice:      None
## Realted doc: None
##
ifeq ($(MTK_OS_CPU_UTILIZATION_ENABLE),y)
COM_CFLAGS         += -DMTK_OS_CPU_UTILIZATION_ENABLE
endif

##
## MTK_AVM_DIRECT
## Brief:       MTK_AVM_DIRECT is a option for Bluetooth Audio.
## Usage:       If the value is "y", the option MTK_AVM_DIRECT will be defined.
## Path:        middleware/MTK/sink
## Dependency:  Must enable the MTK_AVM_DIRECT for bluetooth audio.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_AVM_DIRECT),y)
COM_CFLAGS         += -DMTK_AVM_DIRECT
endif

##
## MTK_AWS_MCE_ENABLE
## Brief:       MTK_AWS_MCE_ENABLE is a option for Bluetooth Audio to enable feature Airoha Wireless Stereo with Multicast Extention.
## Usage:       If the value is "y", the option MTK_AWS_MCE_ENABLE will be defined.
## Path:        middleware/MTK/sink
## Dependency:  MTK_AVM_DIRECT
## Notice:      None
## Relative doc:None
##

##
## MTK_MUX_AWS_MCE_ENABLE
## Brief:       MTK_MUX_AWS_MCE_ENABLE is an option for Bluetooth Audio to enable Airoha Wireless Stereo with Multicast Extention via MUX service.
## Usage:       If the value is "y", the option MTK_MUX_AWS_MCE_ENABLE will be defined.
## Path:       kernel/service/mux
## Dependency:  MTK_AWS_MCE_ENABLE
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_AWS_MCE_ENABLE),y)
COM_CFLAGS         += -DMTK_AWS_MCE_ENABLE
ifeq ($(MTK_MUX_AWS_MCE_ENABLE),y)
COM_CFLAGS         += -DMTK_MUX_AWS_MCE_ENABLE
endif
endif

##
## AIR_PMU_CHARGER_PFM_ENABLE
## Brief:       This option is used to enable pfm for noise enhance.
## Usage:       Enable the feature by configuring it as y.
##              y : enable pfm feature
##              n : disable pfm feature
## Path:        mcu/driver/chip/ab156x/src
## Dependency:  None
## Notice:      None
## Related doc :None
##
ifeq ($(AIR_PMU_CHARGER_PFM_ENABLE),y)
  CFLAGS += -DAIR_PMU_CHARGER_PFM_ENABLE
endif

##
## AIR_USB_ENABLE
## Brief:       This option is to enable and disable USB module.
## Usage:       If the value is "y", the MTK_USB_DEMO_ENABLED compile option will be defined. You must include following module in your Makefile before setting.
##              the option to "y"
##              include the /middleware/MTK/usb/module.mk
## Path:        middleware/MTK/usb
## Dependency:  HAL_USB_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
##
AIR_USB_ENABLE ?= $(MTK_USB_DEMO_ENABLED)
ifeq ($(AIR_USB_ENABLE),y)
COM_CFLAGS         += -DMTK_USB_DEMO_ENABLED
endif

##
## MTK_PORT_SERVICE_ENABLE
## Brief:       This option is to enable port service feature.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/port_service
## Dependency:  NVDM module must be enabled.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_PORT_SERVICE_ENABLE),y)
  CFLAGS += -DMTK_PORT_SERVICE_ENABLE
  CFLAGS += -DMTK_PORT_SERVICE_SLIM_ENABLE
endif

##
## MTK_PORT_SERVICE_BT_ENABLE
## Brief:       This option is to enable port service feature via spp or ble profile in mt2523.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/port_service/src
## Dependency:  Must enable the MTK_PORT_SERVICE_ENABLE for port service feature.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_PORT_SERVICE_ENABLE),y)
ifeq ($(MTK_PORT_SERVICE_BT_ENABLE),y)
  CFLAGS += -DMTK_PORT_SERVICE_BT_ENABLE
endif
endif

##
## MTK_AIRUPDATE_ENABLE
## Brief:       This option is to enable port service feature via airupdate profile in AB156X.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/bt_air/airupdate/src
## Dependency:  Must enable the MTK_PORT_SERVICE_ENABLE and MTK_PORT_SERVICE_BT_ENABLE for port service feature.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_PORT_SERVICE_ENABLE),y)
ifeq ($(MTK_PORT_SERVICE_BT_ENABLE),y)
ifeq ($(MTK_AIRUPDATE_ENABLE),y)
  CFLAGS += -DMTK_AIRUPDATE_ENABLE
endif
endif
endif

##
## MTK_BLE_GAP_SRV_ENABLE
## Brief:       This option is to enable BT GAP LE service feature in AB156X.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/bluetooth_service/src
## Dependency:  None.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_BLE_GAP_SRV_ENABLE),y)
  CFLAGS += -DMTK_BLE_GAP_SRV_ENABLE
endif

##
## MTK_BT_A2DP_AAC_ENABLE
## Brief:       This option is to enable Middleware BT A2DP AAC decoder function under gva/driver/board/mt25x3_hdk/bt_codec folder.
## Usage:       If the value is "y", the MTK_BT_A2DP_AAC_ENABLE compile option will be defined, the sources and header files under gva/driver/board/mt25x3_hdk/bt_codec/src and gva/driver/board/mt25x3_hdk/bt_codec/inc/ will be included by gva/driver/board/mt25x3_hdk/bt_codec/module.mk.
## Path:        gva/driver/board/mt25x3_hdk/bt_codec
## Dependency:  DSP, HAL_AUDIO_MODULE_ENABLED.
## Notice:      None.
## Relative doc:None
##
ifeq ($(MTK_BT_A2DP_AAC_ENABLE),y)
CFLAGS   += -DMTK_BT_A2DP_AAC_ENABLE
endif

##
## MTK_BT_A2DP_VENDOR_ENABLE
## Brief:       This option is to enable Middleware BT A2DP VENDOR decoder function under mcu/middleware/MTK/audio/bt_codec folder.
## Usage:       If the value is "y", the MTK_BT_A2DP_VENDOR_ENABLE compile option will be defined, the sources and header files under mcu/middleware/MTK/audio/bt_codec/src and mcu/middleware/MTK/audio/bt_codec/inc/ will be included by mcu/project/GCC/module.mk.
## Path:        mcu/middleware/MTK/audio/bt_codec
## Dependency:  DSP, HAL_AUDIO_MODULE_ENABLED.
## Notice:      None.
## Relative doc:None
##
ifeq ($(MTK_BT_A2DP_VENDOR_ENABLE),y)
CFLAGS   += -DMTK_BT_A2DP_VENDOR_ENABLE
endif

##
## MTK_HAL_EXT_32K_ENABLE
## Brief:       This option is to determine to use external 32k crystal.
## Usage:       Enable the feature by configuring it as y.
## Path:        driver/chip/mtxxxx
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_HAL_EXT_32K_ENABLE),y)
COM_CFLAGS         += -DMTK_HAL_EXT_32K_ENABLE
endif

##
## MTK_FOTA_ENABLE
## Brief:       This option is to enable and disable FOTA.
## Usage:       If the value is "y", the MTK_FOTA_ENABLE compile option will be defined. You must also include the \middleware\MTK\fota\module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/MTK/fota
## Dependency:  HAL_FLASH_MODULE_ENABLED and HAL_WDT_MODULE_ENABLED must also defined in the hal_feature_config.h  under the project inc folder. If MTK_FOTA_CLI_ENABLE defined in the module.mk is "y", please also include the middleware/MTK/tftp/module.mk. 
## Notice:      Please use the driver/CMSIS/Device/MTK/$(IC_CONFIG)/linkerscript/gcc/full_bin_fota/flash.ld for gcc build environment. 
##              Please use the driver/CMSIS/Device/MTK/$(IC_CONFIG)/linkerscript/rvct/full_bin_fota/flash.sct for KEIL build environment. 
##              Please use the driver/CMSIS/Device/MTK/$(IC_CONFIG)/linkerscript/iar/full_bin_fota/flash.icf for IAR build environment.
## Relative doc:Please refer to the firmware update developer's guide under the doc folder for more detail.
##
#ifeq ($(MTK_FOTA_ENABLE),y)
#CFLAGS += -DMTK_FOTA_ENABLE
#endif

##
## AIR_RACE_CMD_ENABLE
## Brief:       This option is to enable RACE CMD.
## Usage:       Enable the feature by configuring it as y.
## Path:                middleware/MTK/race_cmd
## Dependency:  None
## Notice:      None
## Relative doc:None
##
AIR_RACE_CMD_ENABLE  ?=  $(MTK_RACE_CMD_ENABLE)
ifeq ($(AIR_RACE_CMD_ENABLE),y)
CFLAGS         += -DMTK_RACE_CMD_ENABLE
endif

##
## MTK_RACE_RELAY_CMD_ENABLE
## Brief:       This option is to enable RACE RELAY CMD.
## Usage:       Enable the feature by configuring it as y.
## Path:                middleware/MTK/race_cmd
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifneq ($(MTK_AWS_MCE_ENABLE),y)
MTK_RACE_RELAY_CMD_ENABLE = n
endif

ifeq ($(MTK_RACE_RELAY_CMD_ENABLE),y)
CFLAGS         += -DMTK_RACE_RELAY_CMD_ENABLE
endif

##
## MTK_LINEIN_PLAYBACK_ENABLE
## Brief:       This option is to enable LINEIN PLAYBACK.
## Usage:       Enable the feature by configuring it as y.
## Path:                middleware/MTK/atci/at_command
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_LINEIN_PLAYBACK_ENABLE),y)
CFLAGS         += -DMTK_LINEIN_PLAYBACK_ENABLE
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
CFLAGS += -DAIR_I2S_SLAVE_ENABLE
endif

##
## MTK_PURE_LINEIN_PLAYBACK_ENABLE
## Brief:       This option is to enable PURE LINEIN PLAYBACK.
## Usage:       Enable the feature by configuring it as y.
## Path:                middleware/MTK/audio/linein_playback/src
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_PURE_LINEIN_PLAYBACK_ENABLE),y)
CFLAGS         += -DMTK_PURE_LINEIN_PLAYBACK_ENABLE
endif

##
## MTK_AMP_DC_COMPENSATION_ENABLE
## Brief:       This option is to enable AMP DC COMPENSATION.
## Usage:       Enable the feature by configuring it as y.
## Path:                middleware/MTK/sink/src
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_AMP_DC_COMPENSATION_ENABLE),y)
CFLAGS         += -DMTK_AMP_DC_COMPENSATION_ENABLE
endif

##
## AIR_MBEDTLS_CONFIG_FILE
## Brief:       This option is to configure mbedTLS features.
## Usage:       If the value is "*.h", mbedTLS module will use *.h as the configuration file. For example, if its value is "config-mtk-basic.h",
##              config-mtk-basic.h will be used as the configuration file. MTK_MBEDTLS_CONFIG_FILE compile option will be defined. You must also
##              include the /middleware/third_party/mbedtls/module.mk in your Makefile before setting the option to "*.h".
## Path:        middleware/third_party/mbedtls
## Dependency:  LWIP module must be enabled.
## Relative doc:Please refer to internet and open source software guide under the doc folder for more detail.
##
AIR_MBEDTLS_CONFIG_FILE  ?=  $(MTK_MBEDTLS_CONFIG_FILE)
ifneq ($(AIR_MBEDTLS_CONFIG_FILE),)
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"$(AIR_MBEDTLS_CONFIG_FILE)\"
endif

##
## MTK_ANC_ENABLE
## Brief:       This option is to enable ANC feature.
## Usage:       If the value is "y",  the MTK_ANC_ENABLE option will be defined.
## Path:        middleware/MTK/audio/anc_control/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_ANC_ENABLE),y)
CFLAGS += -DMTK_ANC_ENABLE
CFLAGS += -DHAL_AUDIO_POWER_SLIM_ENABLED
endif

##
## AIR_SIDETONE_USE_PASSTHROUGH_ENABLE
## Brief:       This option is to enable ANC feature.
## Usage:       If the value is "y",  the AIR_SIDETONE_USE_PASSTHROUGH_ENABLE option will be defined.
## Path:        middleware/MTK/audio_manager/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_SIDETONE_USE_PASSTHROUGH_ENABLE),y)
CFLAGS += -DAIR_SIDETONE_USE_PASSTHROUGH_ENABLE
endif

##
## MTK_HYBRID_ANC_ENABLE
## Brief:       This option is to enable support for ANC hybrid mode.
## Usage:       If the value is "y",  the MTK_HYBRID_ANC_ENABLE option will be defined.
## Path:        middleware/MTK/audio/anc_control/
## Dependency:  MTK_ANC_ENABLE
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(MTK_HYBRID_ANC_ENABLE),y)
CFLAGS += -DMTK_HYBRID_ANC_ENABLE
endif
endif

##
## AIR_PEQ_ENABLE
## Brief:       This option is to enable PEQ feature.
## Usage:       If the value is "y",  the MTK_PEQ_ENABLE option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_PEQ_ENABLE),y)
CFLAGS += -DMTK_PEQ_ENABLE
endif

##
## AIR_LINEIN_PEQ_ENABLE
## Brief:       This option is to enable LINEIN PEQ feature.
## Usage:       If the value is "y",  the MTK_LINEIN_PEQ_ENABLE option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_LINEIN_PEQ_ENABLE),y)
CFLAGS += -DMTK_LINEIN_PEQ_ENABLE
endif

##
## MTK_LINEIN_INS_ENABLE
## Brief:       This option is to enable LINEIN INS feature.
## Usage:       If the value is "y",  the MTK_LINEIN_INS_ENABLE option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_LINEIN_INS_ENABLE),y)
CFLAGS += -DMTK_LINEIN_INS_ENABLE
endif

##
## MTK_AUDIO_LOOPBACK_TEST_ENABLE
## Brief:       This option is to enable AUDIO LOOPBACK TEST feature.
## Usage:       If the value is "y",  the MTK_AUDIO_LOOPBACK_TEST_ENABLE option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_AUDIO_LOOPBACK_TEST_ENABLE),y)
CFLAGS += -DMTK_AUDIO_LOOPBACK_TEST_ENABLE
endif

##
## AIR_AUDIO_EXT_DAC_ENABLE
## Brief:       This option is to determine to use external dac.
## Usage:       Enable the feature by configuring it as y.
## Path:        driver/chip/ab156x
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_AUDIO_EXT_DAC_ENABLE),y)
CFLAGS += -DAIR_AUDIO_EXT_DAC_ENABLE
endif

##
## MTK_RECORD_ENABLE
## Brief:       This option is to enable RECORD middleware feature.
## Usage:       If the value is "y",  the MTK_RECORD_ENABLE option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_RECORD_ENABLE),y)
CFLAGS += -DMTK_RECORD_ENABLE
endif

##
## MTK_RECORD_INTERLEAVE_ENABLE
## Brief:       This option is to enable RECORD 2-mic interleave.
## Usage:       If the value is "y",  the MTK_RECORD_INTERLEAVE_ENABLE option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_RECORD_INTERLEAVE_ENABLE),y)
CFLAGS += -DMTK_RECORD_INTERLEAVE_ENABLE
endif

##
## MTK_LEAKAGE_DETECTION_ENABLE
## Brief:       This option is the type of record format selected middleware feature.
## Usage:       If the value is "y",  the MTK_LEAKAGE_DETECTION_ENABLE option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_LEAKAGE_DETECTION_ENABLE),y)
CFLAGS += -DMTK_LEAKAGE_DETECTION_ENABLE
endif

##
## AIR_MP3_CODEC_TASK_DEDICATE_ENABLE
## Brief:       This option is to define MP3 task is always been created.
## Usage:       If the value is "n",  the MTK_MP3_TASK_DEDICATE option will not be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_MP3_CODEC_TASK_DEDICATE_ENABLE),n)
else
CFLAGS += -DMTK_MP3_TASK_DEDICATE
endif

##
## AIR_AUDIO_SUPPORT_MULTIPLE_MICROPHONE_ENABLE
## Brief:       This option is to enable HFP airdump.
## Usage:       If the value is "n",  the HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE option will not be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_AUDIO_SUPPORT_MULTIPLE_MICROPHONE_ENABLE),y)
CFLAGS += -DHAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
CFLAGS += -DENABLE_2A2D_TEST
endif

##
## AIR_MP3_STEREO_SUPPORT_ENABLE
## Brief:       This option is to define MP3 CODEC support stereo format.
## Usage:       If the value is "y",  the MTK_MP3_STEREO_SUPPORT option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_MP3_STEREO_SUPPORT_ENABLE),n)
else
CFLAGS += -DMTK_MP3_STEREO_SUPPORT
endif

##
## AIR_AIRDUMP_ENABLE
## Brief:       This option is to enable HFP airdump.
## Usage:       If the value is "n",  the MTK_AIRDUMP_EN option will not be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_AIRDUMP_ENABLE),y)
CFLAGS += -DMTK_AIRDUMP_EN
endif

##
## MTK_USER_TRIGGER_FF_ENABLE
## Brief:       This option is for user triggered adaptive FF.
## Usage:       If the value is "n",  the MTK_USER_TRIGGER_FF_ENABLE option will not be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_USER_TRIGGER_FF_ENABLE),y)
ifeq ($(MTK_ANC_ENABLE),y)
CFLAGS += -DMTK_USER_TRIGGER_FF_ENABLE
CFLAGS += -DMTK_USER_TRIGGER_ADAPTIVE_FF_V2
endif
endif

##
## MTK_UI_SHELL_MAX_ACTIVITY_COUNT
## Brief:       This option is to set the max activity count in activity stack.
## Usage:       If the value is set in feature option, the macro MTK_UI_SHELL_MAX_ACTIVITY_COUNT will be defined to the value.
## Path:        middleware/MTK/ui_shell
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_UI_SHELL_MAX_ACTIVITY_COUNT),)
CFLAGS += -DMTK_UI_SHELL_MAX_ACTIVITY_COUNT=40
else
CFLAGS += -DMTK_UI_SHELL_MAX_ACTIVITY_COUNT=$(MTK_UI_SHELL_MAX_ACTIVITY_COUNT)
endif

##
## AIR_PEQ_WRITE_NVKEY_ENABLE
## Brief:       This option is used to enable PEQ sound_mode request write nvk.
## Usage:       This is an option that is distinguished by chip option.
## Path:        None
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_PEQ_WRITE_NVKEY_ENABLE),y)
CFLAGS  += -DAIR_PEQ_WRITE_NVKEY_ENABLE
endif

##
## AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
## Brief:       This option is used to enable dual chip mixing mode master side.
## Usage:       If this value is "y", the DAIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE will be defiend.
## Path:        None
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE),y)
CFLAGS  += -DAIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
endif

##
## AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
## Brief:       This option is used to enable dual chip mixing mode slave side.
## Usage:       If this value is "y", the AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE will be defiend.
## Path:        None
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE),y)
CFLAGS  += -DAIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
endif

##
## AIR_UL_FIX_SAMPLING_RATE_48K
## Brief:       This option is to enable UL sampling rate fixed 48kHz.
## Usage:       If this value is "y", the AIR_UL_FIX_SAMPLING_RATE_48K will be defiend.
## Path:        None
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_UL_FIX_SAMPLING_RATE_48K),y)
CFLAGS  += -DAIR_UL_FIX_SAMPLING_RATE_48K
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
endif

##
## AIR_AUDIO_MIXER_GAIN_ENABLE
## Brief:       This option is used to enable output mixer gain.
## Usage:       If this value is "y", the AIR_AUDIO_MIXER_GAIN_ENABLE will be defiend.
## Path:        None
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_AUDIO_MIXER_GAIN_ENABLE),y)
CFLAGS  += -DAIR_AUDIO_MIXER_GAIN_ENABLE
endif

###############################################################################
##
## The following makefile options are not configurable or only for internal user. They may be removed in the future. 

##
## MTK_HEAP_SIZE_GUARD_ENABLE
## Brief:       Internal use.
##
ifeq ($(MTK_HEAP_SIZE_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
COM_CFLAGS     += -DMTK_HEAP_SIZE_GUARD_ENABLE
endif

##
## MTK_SUPPORT_HEAP_DEBUG
## Brief:       Internal use.
## Notice:      MTK_SUPPORT_HEAP_DEBUG is a option to show heap status (alocatted or free), and will print debug info if any heap crash or heap use overflow, It's for RD internal development and debug. Default should be disabled.
##
ifeq ($(MTK_SUPPORT_HEAP_DEBUG),y)
COM_CFLAGS         += -DMTK_SUPPORT_HEAP_DEBUG
endif
##

MTK_SUPPORT_HEAP_DEBUG_ENABLE ?= $(MTK_SUPPORT_HEAP_DEBUG)
## MTK_SUPPORT_HEAP_DEBUG_ENABLE
## Brief:       Internal use.
## Notice:      MTK_SUPPORT_HEAP_DEBUG_ENABLE is a option to show heap status (alocatted or free), and will print debug info if any heap crash or heap use overflow, It's for RD internal development and debug. Default should be disabled.
##
ifeq ($(MTK_SUPPORT_HEAP_DEBUG_ENABLE),y)
COM_CFLAGS         += -DMTK_SUPPORT_HEAP_DEBUG
endif

##
## MTK_SUPPORT_HEAP_DEBUG_ADVANCED
## Brief:       Internal use.
## Notice:      MTK_SUPPORT_HEAP_DEBUG_ADVANCED is a option to show heap allocater with 5-nested callgraph info, and will print debug info if any heap crash or heap use overflow, It's for RD internal development and debug. Default should be disabled.
##
ifeq ($(MTK_SUPPORT_HEAP_DEBUG_ADVANCED),y)
COM_CFLAGS         += -DMTK_SUPPORT_HEAP_DEBUG_ADVANCED
COM_CFLAGS         += -funwind-tables
#LDFLAGS            += -Wl,--no-merge-exidx-entries
endif

MTK_SUPPORT_HEAP_DEBUG_ADVANCED_ENABLE ?= $(MTK_SUPPORT_HEAP_DEBUG_ADVANCED)
##
## MTK_SUPPORT_HEAP_DEBUG_ADVANCED_ENABLE
## Brief:       Internal use.
## Notice:      MTK_SUPPORT_HEAP_DEBUG_ADVANCED_ENABLE is a option to show heap allocater with 5-nested callgraph info, and will print debug info if any heap crash or heap use overflow, It's for RD internal development and debug. Default should be disabled.
##
ifeq ($(MTK_SUPPORT_HEAP_DEBUG_ADVANCED_ENABLE),y)
COM_CFLAGS         += -DMTK_SUPPORT_HEAP_DEBUG_ADVANCED
COM_CFLAGS         += -funwind-tables
#LDFLAGS            += -Wl,--no-merge-exidx-entries
endif

##
## DC_COMPENSATION_PARA_ENABLE
## Brief:       Internal use.
## Usage:       Enable the feature by configuring it as y.
## Path:                middleware/MTK/sink/src
## Dependency:  None
## Notice:      This option is to enable parameters of DC compensation of ab1568.
## Relative doc:None
##
ifeq ($(AIR_DC_COMPENSATION_PARA_ENABLE),y)
CFLAGS         += -DDC_COMPENSATION_PARA_ENABLE
endif

##
## LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
## Brief:       Internal use.
## Usage:       If the value is "y",  the LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA option will be defined.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA),y)
CFLAGS += -DLINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
endif

##
## MTK_BUILD_SMT_LOAD
## Brief:       Internal use.
##
ifeq ($(MTK_BUILD_SMT_LOAD),y)
COM_CFLAGS += -DMTK_BUILD_SMT_LOAD
endif

#
# IC_CONFIG
# DO NOT USE, software not available.
#

##
## MTK_CPU_NUMBER_0
## Brief:       Internal use.
##
ifeq ($(MTK_CPU_NUMBER_0),y)
COM_CFLAGS += -DMTK_CPU_NUMBER_0
COM_CFLAGS += -DMTK_MAX_CPU_NUMBER_2
endif

##
## MTK_CODE_COVERAGE_ENABLE
## Brief:       Internal use.
##
ifeq ($(MTK_CODE_COVERAGE_ENABLE),y)
AR = $(BINPATH)/arm-none-eabi-cov-ar
CC = $(BINPATH)/arm-none-eabi-cov-gcc
CXX= $(BINPATH)/arm-none-eabi-cov-g++
export GCOV_DIR=$(SOURCE_DIR)
endif

##
## MTK_SWLA_ENABLE
## Brief:       Internal use.
## Usage:     If the value is "y", the MTK_SWLA_ENABLE compile option will be defined. You must also include the gva\kernel\service\module.mk in your Makefile before setting the option to "y".
## Path:       kernel/service
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_SWLA_ENABLE),y)
COM_CFLAGS += -DMTK_SWLA_ENABLE
COM_CFLAGS += -DPRODUCT_VERSION_STR=\"$(PRODUCT_VERSION)\"
endif

##
## ENABLE_HWSRC_CLKSKEW
## Brief:       Internal use.
## Notice:      ENABLE_HWSRC_CLKSKEW is a option for hwsrc clk skew. Default should always be enabled.
##
ifeq ($(ENABLE_HWSRC_CLKSKEW),y)
CFLAGS += -DENABLE_HWSRC_CLKSKEW
endif

##
## MTK_INEAR_ENHANCEMENT
## Brief:       Internal use.
## Notice:      MTK_INEAR_ENHANCEMENT is a option for 1+1 inear enhancement. Default should not be enabled.
##
ifeq ($(MTK_INEAR_ENHANCEMENT_ENABLE),y)
CFLAGS += -DMTK_INEAR_ENHANCEMENT
endif


##
## MTK_DUALMIC_INEAR
## Brief:       Internal use.
## Notice:      MTK_DUALMIC_INEAR is a option for 1+1 inear enhancement. Default should not be enabled.
##
ifeq ($(MTK_DUALMIC_INEAR_ENABLE),y)
CFLAGS += -DMTK_DUALMIC_INEAR
endif


##
## MTK_3RD_PARTY_NR
## Brief:       Internal use.
## Notice:      MTK_3RD_PARTY_NR is a option for MTK_3RD_PARTY_NR. Default should not be enabled.
##
ifeq ($(AIR_3RD_PARTY_NR_ENABLE),y)
CFLAGS += -DMTK_3RD_PARTY_NR
endif

##
## AIR_ECNR_PREV_PART_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ECNR_PREV_PART_ENABLE is a option to support split ECNR function.
##
ifeq ($(AIR_ECNR_PREV_PART_ENABLE),y)
CFLAGS += -DAIR_ECNR_PREV_PART_ENABLE
endif

##
## AIR_ECNR_POST_PART_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ECNR_POST_PART_ENABLE is a option to support split ECNR function.
##
ifeq ($(AIR_ECNR_POST_PART_ENABLE),y)
CFLAGS += -DAIR_ECNR_POST_PART_ENABLE
endif

##
## MTK_SLT_AUDIO_HW
## Brief:       Internal use.
## Notice:      MTK_SLT_AUDIO_HW is a option for MTK_SLT_AUDIO_HW. Default should not be enabled.
##
ifeq ($(MTK_SLT_AUDIO_HW),y)
CFLAGS += -DMTK_SLT_AUDIO_HW
endif

##
## AIR_AUDIO_HW_IO_CONFIG_ENHANCE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_AUDIO_HW_IO_CONFIG_ENHANCE_ENABLE is a option for hardware IO config enhance version. Default should be enabled.
##
AIR_AUDIO_HW_IO_CONFIG_ENHANCE_ENABLE ?= $(MTK_AUDIO_HW_IO_CONFIG_ENHANCE)
ifeq ($(AIR_AUDIO_HW_IO_CONFIG_ENHANCE_ENABLE),y)
CFLAGS += -DMTK_AUDIO_HW_IO_CONFIG_ENHANCE
endif

##
## MTK_AUDIO_DUMP_BY_SPDIF_ENABLE
## Brief:       Internal use.
## Notice:      MTK_AUDIO_DUMP_BY_SPDIF_ENABLE is an option to support audio dump over SPDIF interface.
ifeq ($(MTK_AUDIO_DUMP_BY_SPDIF_ENABLE),y)
CFLAGS += -DMTK_AUDIO_DUMP_BY_SPDIF_ENABLE
endif

##
## AIR_AUDIO_GAIN_SETTING_ENHANCE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_AUDIO_GAIN_SETTING_ENHANCE_ENABLE is a option for analog gain and digital gain enhance version. Default should be enabled.
##
AIR_AUDIO_GAIN_SETTING_ENHANCE_ENABLE ?= $(MTK_AUDIO_GAIN_SETTING_ENHANCE)
ifeq ($(AIR_AUDIO_GAIN_SETTING_ENHANCE_ENABLE),y)
CFLAGS += -DMTK_AUDIO_GAIN_SETTING_ENHANCE
endif

##
## FIXED_SAMPLING_RATE_TO_48KHZ
## Brief:       This option is used to enable Fixed sample rate to 48000 for a2dp/hfp/VP/sidetone.
## Usage:       If the value is "n",  the FIXED_SAMPLING_RATE_TO_48KHZ option will not be defined.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
AIR_FIXED_SAMPLING_RATE_TO_48KHZ_ENABLE ?= $(FIXED_SAMPLING_RATE_TO_48KHZ)
ifeq ($(AIR_FIXED_SAMPLING_RATE_TO_48KHZ_ENABLE),y)
CFLAGS += -DFIXED_SAMPLING_RATE_TO_48KHZ
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
CFLAGS += -DAIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ
endif

##
## AIR_FIXED_SUB_DL_HIGH_RES_ENABLE
## Brief:       Internal use.
## Usage:       If the value is "n",  the AIR_FIXED_SUB_DL_HIGH_RES_ENABLE option will not be defined.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE),y)
CFLAGS += -DAIR_FIXED_SUB_DL_HIGH_RES_ENABLE
endif

##
## AIR_BSP_EINT_KEY_RTC_GPIO_ENABLE
## Brief:       This option is used to enable RTC GPIO in eintkey module.
## Usage:       If the value of AIR_BSP_EINT_KEY_RTC_GPIO_ENABLE is "y",  the RTC GPIO in eint    ke module will be included.
## path: driver\board\component\bsp_eint_key
## Dependency:  MTK_EINT_KEY_ENABLE
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_BSP_EINT_KEY_RTC_GPIO_ENABLE),y)
CFLAGS  += -DAIR_BSP_EINT_KEY_RTC_GPIO_ENABLE
endif


## ENABLE_DVFS_DURING_CALCULATE_SHA256
## Brief:       This option is used to allow frequency escalation when calculating SHA256.
## Usage:       This is an option that is distinguished by chip option.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
CFLAGS += -DENABLE_DVFS_DURING_CALCULATE_SHA256

##
## AIR_AUDIO_DETACHABLE_MIC_ENABLE
## Brief:       This option is for voice detachable mic.
## Usage:       If the value is "n",  the AIR_AUDIO_DETACHABLE_MIC_ENABLE option will not be defined.
## Path:        middleware/MTK/audio_manager/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_AUDIO_DETACHABLE_MIC_ENABLE),y)
CFLAGS += -DAIR_AUDIO_DETACHABLE_MIC_ENABLE
endif

##
## AIR_HFP_DNN_PATH_ENABLE
## Brief:       This option is to enable HFP uplink DNN loopback path.
## Usage:       If the value is "y",  the AIR_HFP_DNN_PATH_ENABLE option will enabled.
## Path:        middleware/MTK/audio/
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_HFP_DNN_PATH_ENABLE),y)
CFLAGS += -DAIR_HFP_DNN_PATH_ENABLE
endif

##
## AIR_WIRED_AUDIO_ENABLE
## Brief:       This option is for voice detachable mic.
## Usage:       If the value is "n",  the AIR_WIRED_AUDIO_ENABLE option will not be defined.
## Path:        middleware/MTK/audio/audio_transmitter
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_WIRED_AUDIO_ENABLE), y)
  CFLAGS  += -DAIR_WIRED_AUDIO_ENABLE
endif

##
## MTK_ANC_SURROUND_MONITOR_ENABLE
## Brief:       Internal use.
## Notice:      MTK_ANC_SURROUND_MONITOR_ENABLE is a option to support ANC surround monitor functions.
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(MTK_ANC_SURROUND_MONITOR_ENABLE),y)
CFLAGS += -DMTK_ANC_SURROUND_MONITOR_ENABLE
endif
endif

##
## AIR_ANC_WIND_DETECTION_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ANC_WIND_DETECTION_ENABLE is a option to support wind detection to suppress noise when ANC on.
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(AIR_ANC_WIND_DETECTION_ENABLE),y)
CFLAGS += -DAIR_ANC_WIND_DETECTION_ENABLE
MTK_ANC_SURROUND_MONITOR_ENABLE = y
CFLAGS += -DMTK_ANC_SURROUND_MONITOR_ENABLE
endif
endif

## AIR_ANC_USER_UNAWARE_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ANC_USER_UNAWARE_ENABLE is a option to get better noise cancellation
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(AIR_ANC_USER_UNAWARE_ENABLE),y)
CFLAGS += -DAIR_ANC_USER_UNAWARE_ENABLE
MTK_ANC_SURROUND_MONITOR_ENABLE = y
CFLAGS += -DMTK_ANC_SURROUND_MONITOR_ENABLE
endif
endif

## AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
##
ifeq ($(MTK_ANC_ENABLE),y)
ifeq ($(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE),y)
CFLAGS += -DAIR_ANC_ENVIRONMENT_DETECTION_ENABLE
MTK_ANC_SURROUND_MONITOR_ENABLE = y
CFLAGS += -DMTK_ANC_SURROUND_MONITOR_ENABLE
endif
endif

##
## FLASH_SIZE_8M
## Brief:       Internal use.
## Notice:      FLASH_SIZE_8M is a option to jude flash size .
##
ifeq ($(FLASH_SIZE_8M),y)
CFLAGS += -DFLASH_SIZE_8M
endif

##
## AIR_HWSRC_TX_TRACKING_ENABLE
## Brief:       Internal use.
## Notice:      AIR_HWSRC_TX_TRACKING_ENABLE is a option to enable i2s slave tx hwsrc tracking mode.
##
ifeq ($(AIR_HWSRC_TX_TRACKING_ENABLE),y)
CFLAGS += -DAIR_HWSRC_TX_TRACKING_ENABLE
endif

##
## AIR_HWSRC_RX_TRACKING_ENABLE
## Brief:       Internal use.
## Notice:      AIR_HWSRC_RX_TRACKING_ENABLE is a option to enable i2s slave rx hwsrc tracking mode.
##
ifeq ($(AIR_HWSRC_RX_TRACKING_ENABLE),y)
CFLAGS += -DAIR_HWSRC_RX_TRACKING_ENABLE
endif

##
## AIR_DUAL_CHIP_I2S_ENABLE
## Brief:       Internal use.
## Notice:      AIR_DUAL_CHIP_I2S_ENABLE is a option to enable i2s of dual chip.
##
ifeq ($(AIR_DUAL_CHIP_I2S_ENABLE),y)
CFLAGS += -DAIR_DUAL_CHIP_I2S_ENABLE
endif

##
## AIR_AUDIO_HW_LOOPBACK_ENABLE
## Brief:       Internal use.
## Notice:      AIR_AUDIO_HW_LOOPBACK_ENABLE is a option to enabled hwlookback.
##
ifeq ($(AIR_AUDIO_HW_LOOPBACK_ENABLE),y)
CFLAGS += -DAIR_AUDIO_HW_LOOPBACK_ENABLE
endif

##
## AIR_DUAL_CHIP_NR_ON_MASTER_ENABLE
## Brief:       Internal use.
## Notice:      AIR_DUAL_CHIP_NR_ON_MASTER_ENABLE is a option to enabled MASTER NR.
##
ifeq ($(AIR_DUAL_CHIP_NR_ON_MASTER_ENABLE),y)
CFLAGS += -DAIR_DUAL_CHIP_NR_ON_MASTER_ENABLE
endif

##
## AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
## Brief:       Internal use.
## Notice:      AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE is a option to enabled analog output L/R offset gain setting.
##
ifeq ($(AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE), y)
CFLAGS += -DAIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
endif

##
## AIR_ULL_VOICE_LOW_LATENCY_ENABLE
## Brief:       Internal use.
## Notice:      AIR_ULL_VOICE_LOW_LATENCY_ENABLE is a option to enable ULL voice low latency mode, default should be disabled.
##
ifeq ($(AIR_ULL_VOICE_LOW_LATENCY_ENABLE),y)
CFLAGS += -DAIR_ULL_VOICE_LOW_LATENCY_ENABLE
endif

##
## AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
## Brief:       Internal use.
## Notice:      AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE is a option to enabled analog output L/R offset gain setting.
##
ifeq ($(AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE), y)
CFLAGS += -DAIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
endif

##
## AIR_A2DP_SYNC_CONTROL_ENABLE
## Brief:       This option is to sync control for A2DP sync stop
## Usage:       Enable the feature by configuring it as y.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_A2DP_SYNC_CONTROL_ENABLE),y)
CFLAGS += -DAIR_A2DP_SYNC_START_ENABLE
CFLAGS += -DAIR_A2DP_SYNC_STOP_ENABLE
endif

##
## AIR_HFP_SYNC_CONTROL_ENABLE
## Brief:       This option is to sync control for HFP sync start/stop
## Usage:       Enable the feature by configuring it as y.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_HFP_SYNC_CONTROL_ENABLE),y)
CFLAGS += -DAIR_HFP_SYNC_START_ENABLE
CFLAGS += -DAIR_HFP_SYNC_STOP_ENABLE
endif

##
## AIR_ADVANCED_PASSTHROUGH_ENABLE
## Brief:       Internal use.
## Usage:       AIR_ADVANCED_PASSTHROUGH_ENABLE is a option to enable Advanced Passthrough.
## Dependency:  None
## Notice:      None
## Relative doc:None
##
# Advanced passthrough
ifeq ($(AIR_ADVANCED_PASSTHROUGH_ENABLE), y)
CFLAGS += -DAIR_ADVANCED_PASSTHROUGH_ENABLE
AIR_UL_FIX_SAMPLING_RATE_48K = y
CFLAGS += -DAIR_UL_FIX_SAMPLING_RATE_48K
endif

###############################################################################

##
## AIR_SILENCE_DETECTION_ENABLE
## Brief:       Internal use.
## Notice:      AIR_SILENCE_DETECTION_ENABLE is a option to enabled silence detection feature.
##
ifeq ($(AIR_SILENCE_DETECTION_ENABLE),y)
CFLAGS += -DAIR_SILENCE_DETECTION_ENABLE
endif

##
## AIR_ULL_DONGLE_LINE_IN_OUT_ENABLE
## Brief:       Internal use.
## Usage:       AIR_ULL_DONGLE_LINE_IN_OUT_ENABLE is a option to enable Line in/out in ULL Dongle
## Dependency:  None
## Notice:      None
## Relative doc:None
##
# Advanced passthrough
ifeq ($(AIR_ULL_DONGLE_LINE_IN_OUT_ENABLE), y)
CFLAGS += -DAIR_ULL_DONGLE_LINE_IN_OUT_ENABLE
endif

#Incldue Path
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/configs

CFLAGS     += -std=gnu99 $(COM_CFLAGS)
CXXFLAGS   += -std=c++11 $(COM_CFLAGS)
