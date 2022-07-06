UI_SHELL_SRC = middleware/MTK/ui_shell

# adpate layer
C_FILES += $(UI_SHELL_SRC)/src/platform/src/FreeRTOS/ui_shell_al_event.c
C_FILES += $(UI_SHELL_SRC)/src/platform/src/FreeRTOS/ui_shell_al_memory.c
C_FILES += $(UI_SHELL_SRC)/src/platform/src/FreeRTOS/ui_shell_al_platform_log.c
C_FILES += $(UI_SHELL_SRC)/src/platform/src/FreeRTOS/ui_shell_al_task.c
C_FILES += $(UI_SHELL_SRC)/src/platform/src/FreeRTOS/ui_shell_al_timer.c

# core
C_FILES += $(UI_SHELL_SRC)/src/core/utils/ui_shell_message_handler.c
C_FILES += $(UI_SHELL_SRC)/src/core/utils/ui_shell_message_queue.c
C_FILES += $(UI_SHELL_SRC)/src/core/utils/ui_shell_delay_message_queue.c
C_FILES += $(UI_SHELL_SRC)/src/core/ui_shell_activity_stack.c
C_FILES += $(UI_SHELL_SRC)/src/core/ui_shell_activity_heap.c
C_FILES += $(UI_SHELL_SRC)/src/core/ui_shell_stack_message_handler.c
C_FILES += $(UI_SHELL_SRC)/src/ui_shell_manager.c

#ut
#C_FILES += $(UI_SHELL_SRC)/src/ut/ui_shell_ut_activity_1_1.c
#C_FILES += $(UI_SHELL_SRC)/src/ut/ui_shell_ut_activity_2_1.c
#C_FILES += $(UI_SHELL_SRC)/src/ut/ui_shell_ut_activity_2_2.c
#C_FILES += $(UI_SHELL_SRC)/src/ut/ui_shell_ut_at_command_simulate_event.c
#C_FILES += $(UI_SHELL_SRC)/src/ut/ui_shell_ut_idle_activity1.c
#C_FILES += $(UI_SHELL_SRC)/src/ut/ui_shell_ut_idle_activity2.c
#C_FILES += $(UI_SHELL_SRC)/src/ut/ui_shell_ut_pre_proc_activity.c
#C_FILES += $(UI_SHELL_SRC)/src/ut/ui_shell_ut_project_main.c

CFLAGS  += -I$(SOURCE_DIR)/$(UI_SHELL_SRC)/inc
CFLAGS  += -I$(SOURCE_DIR)/$(UI_SHELL_SRC)/src/core
CFLAGS  += -I$(SOURCE_DIR)/$(UI_SHELL_SRC)/src/core/utils
CFLAGS  += -I$(SOURCE_DIR)/$(UI_SHELL_SRC)/src/platform/inc
CFLAGS  += -I$(SOURCE_DIR)/$(UI_SHELL_SRC)/src/platform/src/FreeRTOS

