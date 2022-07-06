ifeq ($(PROJ_PATH),)
include feature_65_evb.mk
else
include $(PROJ_PATH)/feature_65_evb.mk
endif

ifneq ($(wildcard $(strip $(SOURCE_DIR))/prebuilt/middleware/MTK/secure_boot/),)
MTK_SECURE_BOOT_ENABLE      = y
MTK_BOOTLOADER_USE_MBEDTLS  = y
MTK_MBEDTLS_CONFIG_FILE     = config-mtk-secure_boot.h
else
MTK_SECURE_BOOT_ENABLE      = n
MTK_BOOTLOADER_USE_MBEDTLS  = n
endif
