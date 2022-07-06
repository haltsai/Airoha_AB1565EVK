Packet Lost Compensation prebuilt module usage guide

Brief:          This module is the Packet Lost Compensation(PLC) prebuilt library.

Usage:          GCC:  For PLC, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/voice_plc/module.mk
                      2) Module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_PLC_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/voice_plc/inc

Dependency:     None

Notice:         MTK_PLC_USE_PIC must be configurate as y on specified GCC/feature.mk.

Relative doc:   None

Example project:None