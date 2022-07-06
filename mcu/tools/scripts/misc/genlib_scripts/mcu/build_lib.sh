#!/bin/bash
CWD=${PWD}
cd ${CWD} && cd driver/chip/mt2822/src_protected/GCC&&make clean&&make&&make prebuilt_install&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project/rtos&&make clean&&make BUILD_COMMAND=libble.a &&make prebuilt_install BUILD_COMMAND=libble.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project/rtos&&make clean&&make BUILD_COMMAND=libble_release.a &&make prebuilt_install BUILD_COMMAND=libble_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt.a &&make prebuilt_install BUILD_COMMAND=libbt.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_release.a &&make prebuilt_install BUILD_COMMAND=libbt_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_a2dp.a &&make prebuilt_install BUILD_COMMAND=libbt_a2dp.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_a2dp_release.a &&make prebuilt_install BUILD_COMMAND=libbt_a2dp_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_hfp.a &&make prebuilt_install BUILD_COMMAND=libbt_hfp.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_hfp_release.a &&make prebuilt_install BUILD_COMMAND=libbt_hfp_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_avrcp.a &&make prebuilt_install BUILD_COMMAND=libbt_avrcp.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_avrcp_release.a &&make prebuilt_install BUILD_COMMAND=libbt_avrcp_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_avrcp_enhance.a &&make prebuilt_install BUILD_COMMAND=libbt_avrcp_enhance.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_avrcp_enhance_release.a &&make prebuilt_install BUILD_COMMAND=libbt_avrcp_enhance_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_avrcp_browse.a &&make prebuilt_install BUILD_COMMAND=libbt_avrcp_browse.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_avrcp_browse_release.a &&make prebuilt_install BUILD_COMMAND=libbt_avrcp_browse_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_spp.a &&make prebuilt_install BUILD_COMMAND=libbt_spp.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_spp_release.a &&make prebuilt_install BUILD_COMMAND=libbt_spp_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_pbapc.a &&make prebuilt_install BUILD_COMMAND=libbt_pbapc.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_pbapc_release.a &&make prebuilt_install BUILD_COMMAND=libbt_pbapc_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_hsp.a &&make prebuilt_install BUILD_COMMAND=libbt_hsp.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_hsp_release.a &&make prebuilt_install BUILD_COMMAND=libbt_hsp_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_hid.a &&make prebuilt_install BUILD_COMMAND=libbt_hid.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_hid_release.a &&make prebuilt_install BUILD_COMMAND=libbt_hid_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_aws.a &&make prebuilt_install BUILD_COMMAND=libbt_aws.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_aws_release.a &&make prebuilt_install BUILD_COMMAND=libbt_aws_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_aws_mce.a &&make prebuilt_install BUILD_COMMAND=libbt_aws_mce.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_aws_mce_release.a &&make prebuilt_install BUILD_COMMAND=libbt_aws_mce_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_airupdate.a &&make prebuilt_install BUILD_COMMAND=libbt_airupdate.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbt_airupdate_release.a &&make prebuilt_install BUILD_COMMAND=libbt_airupdate_release.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbtdriver_2822.a &&make prebuilt_install BUILD_COMMAND=libbtdriver_2822.a&&make clean
cd ${CWD} && cd middleware/MTK/hummingbird/project_duo/rtos&&make clean&&make BUILD_COMMAND=libbtdriver_2822_release.a &&make prebuilt_install BUILD_COMMAND=libbtdriver_2822_release.a&&make clean
cd ${CWD} && cd middleware/MTK/pikachu/btcon&&make clean&&make BUILD_COMMAND=libpka_2822.a&&make prebuilt_install BUILD_COMMAND=libpka_2822.a&&make clean
cd ${CWD} && cd middleware/MTK/ama_protected/GCC&&make clean&&make -j8 fm=../../../../project/mt2822/apps/earbuds_ref_design/GCC/feature_mt2822s_evb.mk&&make prebuilt_install fm=../../../../project/mt2822/apps/earbuds_ref_design/GCC/feature_mt2822s_evb.mk&&make clean
cd ${CWD} && cd middleware/MTK/bt_fast_pair_protected/GCC&&make clean&&make -j8&&make prebuilt_install&&make clean
cd ${CWD} && cd middleware/MTK/mfi_coprocessor_protected/GCC&&make clean&&make -j8&&make prebuilt_install&&make clean
cd ${CWD} && cd middleware/MTK/ble_ancs_protected/GCC&&make clean&&make -j8&&make prebuilt_install&&make clean
cd ${CWD} && cd middleware/MTK/iap2_protected/GCC&&make clean&&make -j8 fm=../../../../project/mt2822/apps/earbuds_ref_design/GCC/feature_mt2822s_evb.mk&&make prebuilt_install fm=../../../../project/mt2822/apps/earbuds_ref_design/GCC/feature_mt2822s_evb.mk&&make clean
