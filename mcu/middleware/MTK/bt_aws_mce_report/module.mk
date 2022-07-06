###################################################
# Sources
###################################################

BT_AWS_MCE_REPORT_SOURCE = middleware/MTK/bt_aws_mce_report/src
BT_AWS_MCE_REPORT_FILES =  $(BT_AWS_MCE_REPORT_SOURCE)/bt_aws_mce_report.c


C_FILES += $(BT_AWS_MCE_REPORT_FILES)
###################################################
# include path
###################################################

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bt_aws_mce_report/inc
