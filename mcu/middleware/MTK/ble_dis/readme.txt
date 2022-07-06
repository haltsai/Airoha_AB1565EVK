ble_dis module usage guide

Brief:          This module is the Device Information Service (DIS) implementation. DIS exposes 
manafacturer and/or vendor information about a device.a cntrol point to allow a peer device to cause the 
                local device to immediately alert by a Bluetooth low-energy link. This module manages all user's registered callbacks and notify
                all users when Alert Lvel related write event occurs from the peer device.
Usage:          GCC: 1. Include the module with "include $(SOURCE_DIR)/middleware/MTK/ble_dis/module.mk" in the GCC project Makefile
                KEIL: 1. Drag the middleware/MTK/ble_dis folder to the project.
                      2. Add middleware/MTK/ble_dis/inc to include paths.
                IAR: 1. Drag the middleware/MTK/ble_dis folder to the project.
                     2. Add middleware/MTK/ble_dis/inc to include paths.
Dependency:     This module depends on Bluetooth. Please set MTK_BT_ENABLE to be "y" in feature.mk and include Bluetooth module.mk in your project.
Notice:         None.
Relative doc:   None.
Example project:project/mt2533_evb/apps/bt_headset_gui_ref_design/src/ble_find_me_server.


