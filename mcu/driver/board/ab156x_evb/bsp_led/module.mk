
###################################################
BOARD_LED_DIR = driver/board/$(BOARD_CONFIG)/bsp_led
###################################################
# include path
CFLAGS  += -I$(SOURCE_DIR)/$(BOARD_LED_DIR)/inc
#source file
C_FILES += $(BOARD_LED_DIR)/src/bsp_led_isink.c
C_FILES += $(BOARD_LED_DIR)/src/bsp_led.c
#################################################################################
CFLAGS         += -DMTK_LED_ENABLE
