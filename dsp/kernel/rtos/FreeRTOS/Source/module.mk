###################################################
# C Sources
C_SRC  +=  kernel/rtos/FreeRTOS/Source/event_groups.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/list.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/queue.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/tasks.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/timers.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/portable/XCC/Xtensa/port.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/portable/XCC/Xtensa/$(IC_CONFIG)/port_tick.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/portable/XCC/Xtensa/portclib.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/portable/XCC/Xtensa/xtensa_init.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/portable/XCC/Xtensa/xtensa_overlay_os_hook.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/portable/Trace/os_trace_callback.c
C_SRC  +=  kernel/rtos/FreeRTOS/Source/portable/Trace/os_port_callback.c
###################################################
# Include Path
INC  +=  kernel/rtos/FreeRTOS/Source/portable/XCC/Xtensa
INC  +=  kernel/rtos/FreeRTOS/Source/portable/XCC/Xtensa/$(IC_CONFIG)
INC  +=  kernel/rtos/FreeRTOS/Source/include
INC  +=  kernel/rtos/FreeRTOS/Source/portable/Trace
###################################################
# Include sub-module
include $(ROOTDIR)/kernel/rtos/FreeRTOS/Source/portable/MemMang/multi_pool/module.mk

#################################################################################
#Enable the feature by configuring
CCFLAG += -DFREERTOS_ENABLE
ASFLAG += -DFREERTOS_ENABLE
CCFLAG += -DXT_BOARD


