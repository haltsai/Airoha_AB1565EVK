Record module usage guide

Brief:          This module is the Record control implementation.

Usage:          GCC:  For record, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified GCC/feature.mk.
                         MTK_RECORD_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/record_control/inc

Dependency:     None.

Notice:         None.

Relative doc:   None.

Example project:None.
