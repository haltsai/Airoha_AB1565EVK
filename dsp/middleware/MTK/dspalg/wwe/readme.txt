WWE module usage guide

Brief:          This module is the WWE implementation for audio. It provides support for AMA/GSOUND wake word detection.

Usage:          XT-XCC:  For WWE, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/wwe/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_WWE_ENABLE
                         MTK_WWE_USE_PIC
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/wwe/inc
                      4) Add FUNC_WWE_PREPROC/FUNC_WWE_PROC into the feature table in dsp_sdk.c to apply WWE in the specified scenario, like stream_feature_list_wwe_mic_record[].

Dependency:     None
                for changing WWE coefficient synchronization in earbuds project.

Notice:         1) MTK_WWE_USE_PIC must be set as "y" on the specified XT-XCC/feature.mk.

Relative doc:   None

Example project: None
