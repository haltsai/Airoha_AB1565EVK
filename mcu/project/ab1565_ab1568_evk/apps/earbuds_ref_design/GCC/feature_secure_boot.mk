ifeq ($(PROJ_PATH),)
include feature_65_evk.mk
else
include $(PROJ_PATH)/feature_65_evk.mk
endif

ifneq ($(wildcard $(strip $(SOURCE_DIR))/prebuilt/middleware/MTK/secure_boot/),)
MTK_SECURE_BOOT_ENABLE       = y
BL_FEATURE = feature_secure_boot.mk
else
MTK_SECURE_BOOT_ENABLE      = n
endif

MTK_BT_A2DP_VENDOR_ENABLE = n

#HW IO config enhance
MTK_AUDIO_HW_IO_CONFIG_ENHANCE = y

#Gain setting enhance
MTK_AUDIO_GAIN_SETTING_ENHANCE = y

# UI shell max activity count in activity stack.
MTK_UI_SHELL_MAX_ACTIVITY_COUNT	= 40

