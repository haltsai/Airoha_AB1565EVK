ANC module usage guide

Brief:          This module is the Active Noise Suppression (ANC) control implementation. It also supports passthrough with the same hardware module.

Usage:          GCC:  For ANC, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/anc_control/module.mk
                      2) Module.mk provides different options to enable or disable according to the profile.
                         Please configure the related options on the specified GCC/feature.mk.
                         MTK_ANC_ENABLE
                         MTK_HYBRID_ANC_ENABLE
                         MTK_ANC_BACKUP_STATUS_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/anc_control/inc

Dependency:     None

Notice:         None

Relative doc:   None

Example project:None
