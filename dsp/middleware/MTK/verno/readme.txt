verno module usage guide

Brief:          This module is used to record the build date so that it is easy to identify different versions of the DSP.

Usage:          XT-XCC:  For verno, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/verno/module.mk
                      2) Module.mk provides different options to enable or disable according profiles, please configure these options on the specified XT-XCC/feature.mk:
                         None
                      3) Add the header file path:
                         None
                      4) extern char build_date_time_str[] in the file in which you want to print the build date information.

Dependency:     None

Notice:         None

Relative doc:   None

Example project: None
