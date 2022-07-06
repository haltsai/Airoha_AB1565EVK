###################################################
# Sources
###################################################


# LE Association
BT_MCE_LE_ASSOCIATION_SRC = middleware/MTK/bt_association/src
C_FILES  += $(BT_MCE_LE_ASSOCIATION_SRC)/bt_aws_mce_le_association_service.c \
            $(BT_MCE_LE_ASSOCIATION_SRC)/bt_aws_mce_le_association_client.c \
			$(BT_MCE_LE_ASSOCIATION_SRC)/bt_aws_mce_ls_association.c \
            $(BT_MCE_LE_ASSOCIATION_SRC)/bt_aws_mce_le_association_common.c

###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bt_association/inc



