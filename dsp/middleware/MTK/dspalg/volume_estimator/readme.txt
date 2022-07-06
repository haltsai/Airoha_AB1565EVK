Volume estimator prebuilt module usage guide

Brief:          This module is the volume estimator  prebuilt library.

Usage:          GCC:  For volume estimator , include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/dspalg/volume_estimator/module.mk
                      2) module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         AIR_VOLUME_ESTIMATOR_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dspalg/volume_estimator/inc

Dependency:     None.

Notice:         AIR_VOLUME_ESTIMATOR_ENABLE must be configurate as y on specified GCC/feature.mk.

Relative doc:   None.

Example project:None.