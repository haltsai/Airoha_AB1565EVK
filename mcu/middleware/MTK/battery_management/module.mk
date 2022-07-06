
ifeq ($(IC_CONFIG), $(filter $(IC_CONFIG),mt2523 mt2523s))
###################################################

BMT_SRC = middleware/MTK/battery_management

BMT_FILES = $(BMT_SRC)/port/mt2523/src/battery_management_driver.c  \
			$(BMT_SRC)/port/mt2523/src/battery_management_meter.c   \
			$(BMT_SRC)/port/mt2523/src/battery_management_core.c

C_FILES += $(BMT_FILES) 

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/fuelgauge/inc

# include library
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/fuelgauge/lib/libfgauge.a

endif

ifeq ($(IC_CONFIG),mt2533)
###################################################

ifeq ($(MTK_EXTERNAL_PMIC), y)
# external gague files
ifeq ($(MTK_EXTERNAL_GAUGE), y)
# gague driver files
include $(SOURCE_DIR)/driver/board/component/gauge/module.mk
CFLAGS	+= -DMTK_EXTERNAL_GAUGE
endif
endif

BMT_SRC = middleware/MTK/battery_management

BMT_FILES = $(BMT_SRC)/src/battery_management.c \
			$(BMT_SRC)/port/mt2533/src/battery.c

C_FILES += $(BMT_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/mt2533/inc

endif

ifeq ($(IC_CONFIG),ab155x)
BMT_SRC = /middleware/MTK/battery_management

BMT_FILES = $(BMT_SRC)/src/battery_management.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_core.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_charger_mt6388_api.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_power_saving.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_BC12.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_charger_api.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_gauge.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_auxadc.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_HW_JEITA.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_BJT_charging.c \
			$(BMT_SRC)/port/ab155x/src/battery_management_efuse.c \
			
C_FILES += $(BMT_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/ab155x/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/pmic
CFLAGS += -DMTK_BATTERY_MANAGEMENT_ENABLE

else ifeq ($(IC_CONFIG),mt2822)
BMT_SRC = /middleware/MTK/battery_management

BMT_FILES = $(BMT_SRC)/src/battery_management.c \
			$(BMT_SRC)/port/mt2822/src/battery_management_core.c \
			$(BMT_SRC)/port/mt2822/src/battery_management_charger_api.c \
			$(BMT_SRC)/port/mt2822/src/battery_management_gauge.c \
			$(BMT_SRC)/port/mt2822/src/battery_management_HW_JEITA.c \
			$(BMT_SRC)/port/mt2822/src/battery_management_auxadc.c \
			
						
C_FILES += $(BMT_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/mt2822/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/mt2822/inc/fuel_gauge
CFLAGS += -I$(SOURCE_DIR)/driver/board/componen
CFLAGS += -DMTK_BATTERY_MANAGEMENT_ENABLE

else ifeq ($(IC_CONFIG),ab156x)
BMT_SRC = /middleware/MTK/battery_management

ifeq ($(IC_TYPE),ab1565)
BMT_FILES = $(BMT_SRC)/src/battery_management.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_core_2565.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_charger_api.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_gauge.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_HW_JEITA.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_auxadc.c \
            $(BMT_SRC)/port/ab156x/src/battery_management_sw_ntc.c \
			
C_FILES += $(BMT_FILES)
endif

ifeq ($(IC_TYPE),ab1568)
BMT_FILES = $(BMT_SRC)/src/battery_management.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_core_2568.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_charger_api.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_gauge.c \
			$(BMT_SRC)/port/ab156x/src/battery_management_HW_JEITA.c \
            $(BMT_SRC)/port/ab156x/src/battery_management_auxadc.c \
			
C_FILES += $(BMT_FILES)
endif

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/ab156x/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/componen
CFLAGS += -DMTK_BATTERY_MANAGEMENT_ENABLE
endif

ifeq ($(IC_CONFIG),am255x)
BMT_SRC = /middleware/MTK/battery_management

BMT_FILES = $(BMT_SRC)/src/battery_management.c \
			$(BMT_SRC)/port/am255x/src/battery_management_core.c \
			$(BMT_SRC)/port/am255x/src/battery_management_charger_mt6388_api.c \
			$(BMT_SRC)/port/am255x/src/battery_management_power_saving.c \
			$(BMT_SRC)/port/am255x/src/battery_management_BC12.c \
			$(BMT_SRC)/port/am255x/src/battery_management_charger_am255x_api.c \
			$(BMT_SRC)/port/am255x/src/battery_management_gauge.c \
			$(BMT_SRC)/port/am255x/src/battery_management_auxadc.c \
			$(BMT_SRC)/port/am255x/src/battery_management_HW_JEITA.c \
			$(BMT_SRC)/port/am255x/src/battery_management_BJT_charging.c \
			$(BMT_SRC)/port/am255x/src/battery_management_efuse.c \
			
C_FILES += $(BMT_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/am255x/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/pmic
CFLAGS += -DMTK_BATTERY_MANAGEMENT_ENABLE
endif

