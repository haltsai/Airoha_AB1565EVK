bluetooth_service module usage guide

Brief:          This module is the bluetooth common services implementation. It includes 3 services, one is Device Manager Service, which manages bonded peer 
                device's security information, and the other is GATT Server Service, which supports GAP service and characteristics configuration 
                feature, and the last is BT GAP LE Service, which manages the LE advertising operation and the device's connection information and connection parameters.
Usage:          GCC: 1. Include the module with "include $(SOURCE_DIR)/middleware/MTK/bluetooth_service/module.mk" in the GCC project Makefile
                KEIL: 1. Drag the middleware/MTK/bluetooth_service folder to the project.
                      2. Add middleware/MTK/bluetooth_service/inc to include paths.
                IAR: 1. Drag the middleware/MTK/bluetooth_service folder to the project.
                     2. Add middleware/MTK/bluetooth_service/inc to include paths.
Dependency:     This module depends on Bluetooth. Please set MTK_BT_ENABLE to be "y" in feature.mk and include Bluetooth module.mk in your project.
                Please MTK_BLE_GAP_SRV_ENABLE to be "y" in feature.mk if you want to use the BT GAP LE service.
Notice:         None.
Relative doc:   None.
Example project:project/mt2533_evb/apps/bt_headset_gui_ref_design/src/ble_find_me_server.


