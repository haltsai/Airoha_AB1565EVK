###################################################
# Sources
###################################################

ifeq ($(MTK_BT_CM_SUPPORT), y)
BT_CONNECTION_MANAGER_SOURCE = middleware/MTK/bt_connection_manager/src
C_FILES	 += $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_adapt.c
ifeq ($(MTK_AWS_MCE_ENABLE), y)
C_FILES	 += $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_aws_mce.c
endif
else
BT_CONNECTION_MANAGER_SOURCE = middleware/MTK/bt_connection_manager/legacy/src
C_FILES	 += $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_device_local_info.c
endif

C_FILES	 += $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_config.c \
            $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager.c \
            $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_utils.c \
            $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_atci_cmd.c \
            $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_power.c

ifeq ($(MTK_AWS_MCE_ENABLE), y)
C_FILES  += $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_aws_air_pairing.c \
            $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_aws_rho.c
endif

ifeq ($(MTK_AWS_MCE_ROLE_RECOVERY_ENABLE), y)
ifeq ($(MTK_BT_CM_SUPPORT), y)
C_FILES  += $(BT_CONNECTION_MANAGER_SOURCE)/bt_aws_mce_role_recovery.c
else
C_FILES  += $(BT_CONNECTION_MANAGER_SOURCE)/bt_connection_manager_state_machine_ext.c
endif
endif

###################################################
# include path
###################################################
ifeq ($(MTK_BT_CM_SUPPORT), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bt_connection_manager/inc
CFLAGS	+= -DMTK_BT_CM_SUPPORT
else
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bt_connection_manager/legacy/inc
endif
 
