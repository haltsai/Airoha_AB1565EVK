Bluetooth role handover service module usage guide

Brief:           This module is to manage all Bluetooth role handover users. User can register and deregister their  
                 callback functions to complete the role handover procedure. It is only supported on AB155X.
Usage:           GCC: 
                      1) Add the following module.mk for include path and source file:
                         include $(SOURCE_DIR)/middleware/MTK/bt_role_handover/module.mk
                         in your GCC project Makefile.

                 KEIL: 
                       1) Add all source files under the path "middleware/MTK/bt_role_handover/src" to your
                          project.
                       2) Add the path "..\..\..\..\..\middleware\MTK\bt_role_handover\inc" to the "include paths" of the C/C++ options.

                 IAR: 
                      1) Add all source files under the path "middleware/MTK/bt_role_handover/src" to your project. 
                      2) Add the path "$PROJ_DIR$\..\..\..\..\..\middleware\MTK\bt_role_handover\inc" to the "Additional include directories" of the preprocessor.

Dependency:      This module has the dependency with Bluetooth, AVM direct feature and AWS MCE profile. Please also make sure to include Bluetooth.
                 MTK_BT_ENABLE, MTK_AVM_DIRECT and MTK_AWS_MCE_ENABLE must set to 'y' in your project.
Notice:          None.
Relative doc:    None.
Example project: Please find the project earbuds_ref_design under project/ab155x_evk/apps folder.
