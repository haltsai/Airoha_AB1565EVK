Bluetooth fast pair 2.0 module usage guide

Brief:           This module is used to provide google fast pair 2.0 feature. User can call API or send action provide by
							   this module to fast pair 2.0 feature.
Usage:           GCC: 
                      1) Add the following module.mk for include path and source file:
                         include $(SOURCE_DIR)/middleware/MTK/bt_fast_pair/module.mk
                         in your GCC project Makefile.

                 KEIL: 
                       1) Add all source files under the path "middleware/MTK/bt_fast_pair/src" to your
                          project.
                       2) Add the path "..\..\..\..\..\middleware\MTK\bt_fast_pair\inc" to the "include paths" of the C/C++ options.

                 IAR: 
                      1) Add all source files under the path "middleware/MTK/bt_fast_pair/src" to your project. 
                      2) Add the path "$PROJ_DIR$\..\..\..\..\..\middleware\MTK\bt_fast_pair\inc" to the "Additional include directories" of the preprocessor.

Dependency:      This module depends on Bluetooth. Please set MTK_BT_ENABLE to be "y" in feature.mk and include Bluetooth module.mk in your project.
Notice:          None.
Relative doc:    None.
Example project: Please find the project earbuds_ref_design under project/ab155x_evk/apps folder.
