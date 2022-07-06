#!/bin/bash

set -e
set -o pipefail
###############################################################################
#Variables
SOURCE_DIR=$1
OUTPUT=$2
IC_CONFIG=$3
BOARD_CONFIG=$4
BIN_FILENAME=$5
PROJ_DIR=$6
SECURE_BOOT_EN=$7
FLASHBIN_NAME=flash.bin
BOOTLOADERMAXSIZE=65536  # 64KB

# Read feature option from copy_firmware_opts.log, which is generated in .rule.mk.
# Please add the feature option you want to use to tools/script/build/copy_firmware_opts.lis.
source "${OUTPUT}/copy_firmware_opts.log"

# remove keyword  "cust_" in variable ${BOARD_TYPE}
BOARD_TYPE=${BOARD_TYPE//cust_/}
###############################################################################
#Functions
get_board () {
    i=`pwd | awk -F"/" '{print NF}'`
    while [ $i -gt 0 ]; do
        last_name=${item_name}
        item_name=`pwd | awk -F"/" '{print $'$i'}'`
        if [ "${item_name}" == "project" ]; then
            BOARD=${last_name}
            break
        fi
        let i=i-1
    done
    echo BOARD=${BOARD}
    if [ "${BOARD}" == "" ]; then
        echo "Error: It cannot find board name"
        exit 1
    fi
}

change_bin_filename () {

    awk \
    '{ \
        if ($0 ~ /\[Section.+\]/) { \
            if ($0 ~ /\[Section2\]/) { \
                sect2=1 \
            } else { \
                sect2=0 \
           } \
        } \
    \
        if (sect2==1 && $0~/Location=/) { \
            print "Location="'\"${2}\"' \
        } else { \
            print $0 \
        } \
    }' ${1} > ${1}.tmp

    mv "${1}.tmp" "${1}"

}


change_cfg_bin_filename () {
    sed -i "s|freertos_test.bin|${2}|g" ${1}
    sed -i "s|iot_sdk.bin|${2}|g" ${1}
    sed -i "s|\bfreertos_create_thread.bin|${2}|g" ${1}
    if [ ! -z "${PRODUCT_CATEGORY}" ]; then
        sed -i "/^general:/a\    product_category: ${PRODUCT_CATEGORY}" ${1}
    fi
    # comment dsp1 part
     sed -i -n '{/rom:/{x;n;{/dsp1/{x;s|^|#|;p;x;tc};x;p;be}};{:c;/\bdsp1_/,+2s|^|#|;}};p;bend;:e;x;p;:end' ${1}
    # comment dsp0 part
     sed -i -n '{/rom:/{x;n;{/dsp0/{x;s|^|#|;p;x;tc};x;p;be}};{:c;/\bdsp0_/,+2s|^|#|;}};p;bend;:e;x;p;:end' ${1}

    # comment nvdm part if  nvdm.bin not exist
     if [ ! -e "${OUTPUT}/nvdm.bin" ]; then
         sed -i -n '{/rom:/{x;n;{/nvdm/{x;s|^|#|;p;x;tc};x;p;be}};{:c;/\bnvdm/,+2s|^|#|;}};p;bend;:e;x;p;:end' ${1}
     fi

    # comment nvdm part if  filesystem.bin not exist
     if [ ! -e "${OUTPUT}/filesystem.bin" ]; then
         sed -i -n '{/rom:/{x;n;{/filesystem/{x;s|^|#|;p;x;tc};x;p;be}};{:c;/\bfilesystem/,+2s|^|#|;}};p;bend;:e;x;p;:end' ${1}
     fi
}

change_cfg_hfp_filename () {
    sed -i "s|ab155x_patch_e1_hdr.bin|ab155x_patch_e1_hdr_aws_hfp.bin|g" ${1}
}

change_begin_end_address_filename () {

    awk \
    '{ \
        if ($0 ~ /\[Section.+\]/) { \
            if ($0 ~ /\[Section2\]/) { \
                sect2=1 \
            } else { \
                sect2=0 \
           } \
        } \
    \
        if (sect2==1 && $0~/BeginAddress=/) { \
            print "BeginAddress="'\"${2}\"' \
        } else if (sect2==1 && $0~/EndAddress=/) { \
            print "EndAddress="'\"${3}\"' \
        } else { \
            print $0 \
        } \
    }' ${1} > ${1}.tmp

    mv "${1}.tmp" "${1}"
}

# Combine bootloader and CM4 Firmware if it's neccessary
combine_bl_cm4 () {
    echo "Start combining bootloader & CM4 firmware..."
    CM4_FRIMWARE_BIN=${1}/${2}
    BOOTLOADER_BIN=${1}/${3}
    FLASHBIN=${1}/${4}
    echo "CM4_FRIMWARE_BIN = ${CM4_FRIMWARE_BIN}"
    echo "BOOTLOADER_BIN = ${BOOTLOADER_BIN}"
    echo "FLASHBIN = ${FLASHBIN}"
    if [ -f "${FLASHBIN}" ]; then
        echo "delete ${FLASHBIN}"
        rm "${FLASHBIN}"
    fi
    BOOTLOADER_BIN_TMP=${1}/`basename ${BOOTLOADER_BIN} .bin`.tmp
    cp -f "${BOOTLOADER_BIN}" "${BOOTLOADER_BIN}_TMP"
    BOOTLOADER_BIN_SIZE=`wc --bytes ${BOOTLOADER_BIN} | cut -d ' ' -f1`
    PADDINGCOUNT=$(($BOOTLOADERMAXSIZE-$BOOTLOADER_BIN_SIZE))
    printf '\377%.0s' $(eval echo "{1..${PADDINGCOUNT}}")  >> ${BOOTLOADER_BIN_TMP}
    cat ${BOOTLOADER_BIN_TMP} ${CM4_FRIMWARE_BIN} > ${FLASHBIN}
    rm "${BOOTLOADER_BIN_TMP}"
    echo "Done for combining bootloader & CM4 firmware..."
}

combine_btN9_bins(){
  # the last argument is destitnation
  #echo "Function name:  ${FUNCNAME}"
  #echo "The number of positional parameter : $#"
  #echo "All parameters or arguments passed to the function: '$@'"
  theFirstBin="${1}"
  numberOfBinsAppended="$(($#-2))"
  destinationOfBin="${@:$#}"
  #echo "combine_btN9_bins : $numberOfBinsAppended"
  #echo "\$PWD : ${PWD}"
  cat ${@:1:$#-1} > ${destinationOfBin}
  echo -n -e "\x0${numberOfBinsAppended}" > ${OUTPUT}/tmp && dd if=${OUTPUT}/tmp of=${destinationOfBin} bs=1 seek=7 count=1 conv=notrunc && rm -f "${OUTPUT}/tmp"
}

###############################################################################
#Begin here
echo `basename ${0}`"...."
get_board

#echo "=============================\$PROJ_NAME :$PROJ_NAME ==============================="

if [ "${BOARD_CONFIG}" == "mt2523_hdk" ]; then
    # copy download config file
   if [ "${MTK_GNSS_ENABLE}" == 'y' ]; then
        if [ ! -f "${SOURCE_DIR}/prebuilt/middleware/MTK/gnss/firmware/gnss_chip_fw.bin" ]; then
            cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/default/flash_download.cfg" "${OUTPUT}/"
        else
            cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/gnss/flash_download.cfg" "${OUTPUT}/"
            cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/gnss/firmware/gnss_chip_fw.bin" "${OUTPUT}/gnss_firmware.bin"
        fi
    elif [ "${MTK_WIFI_STUB_CONF_ENABLE}" == 'y' ]; then
        if [ -f "${SOURCE_DIR}/prebuilt/driver/chip/mt5932/wifi_image/mt5932_image_sdio.bin" ]; then
            cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/wifi/flash_download.cfg" "${OUTPUT}/"
            cp -f "${SOURCE_DIR}/prebuilt/driver/chip/mt5932/wifi_image/mt5932_image_sdio.bin" "${OUTPUT}/wifi_firmware.bin"
         else
           cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/wifi/flash_download.cfg" "${OUTPUT}/"
        fi
    else
        cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/default/flash_download.cfg" "${OUTPUT}/"
    fi

    # copy bootloader
    if [ ! -f "${OUTPUT}/${IC_CONFIG}_bootloader.bin" ]; then
        cp -f "${SOURCE_DIR}/project/${BOARD_CONFIG}/apps/bootloader/GCC/bootloader.bin" "${OUTPUT}/${IC_CONFIG}_bootloader.bin"
        echo "${OUTPUT}/${IC_CONFIG}_bootloader.bin doesn't exist. copy default bootloader done."
    fi
    if [ "${WITH_BOOTLOADER}" != "n" ]; then
        combine_bl_cm4 ${OUTPUT} ${BIN_FILENAME} ${IC_CONFIG}_bootloader.bin ${FLASHBIN_NAME}
    fi

elif [ "${BOARD_CONFIG}" == "mt2533_evb" ]; then
    # copy download config file
    if [ "$MTK_GNSS_ENABLE" == 'y' ]; then
        cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/gnss/flash_download.cfg" "${OUTPUT}/"
    else
        cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/default/flash_download.cfg" "${OUTPUT}/"
    fi

    # copy bootloader
    if [ ! -f "${OUTPUT}/${IC_CONFIG}_bootloader.bin" ]; then
        cp -f "${SOURCE_DIR}/project/${BOARD_CONFIG}/apps/bootloader/GCC/bootloader.bin" $"{OUTPUT}/$IC_CONFIG_bootloader.bin"
        echo "${OUTPUT}/${IC_CONFIG}_bootloader.bin doesn't exist. copy default bootloader done."
    fi
    if [ "$WITH_BOOTLOADER" != "n" ]; then
        combine_bl_cm4 ${OUTPUT} ${BIN_FILENAME} "${IC_CONFIG}_bootloader.bin" ${FLASHBIN_NAME}
    fi
elif [ "${BOARD_CONFIG}" == "mt2523s_headset" ]; then
    # copy download config file
    cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/default/flash_download.cfg" "${OUTPUT}/"

    # copy bootloader
    if [ ! -f "${OUTPUT}/${IC_CONFIG}_bootloader.bin" ]; then
        cp -f "${SOURCE_DIR}/project/${BOARD_CONFIG}/apps/bootloader/GCC/bootloader.bin" "${OUTPUT}/$IC_CONFIG_bootloader.bin"
        echo "${OUTPUT}/${IC_CONFIG}_bootloader.bin doesn't exist. copy default bootloader done."
    fi
    if [ "$WITH_BOOTLOADER" != "n" ]; then
        combine_bl_cm4 ${OUTPUT} ${BIN_FILENAME} "${IC_CONFIG}_bootloader.bin" ${FLASHBIN_NAME}
    fi
elif [ "${BOARD_CONFIG}" == "mt7697_hdk" ] || [ "${BOARD_CONFIG}" == "linkit7697_hdk" ]; then

    echo "bin filename is ${BIN_FILENAME}"
    if [ "${BIN_FILENAME}" == 'iot_sdk_lite.bin' ]; then
        echo "copy 2M flash ini"
        cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/flash_download_2M.ini" "${OUTPUT}/flash_download.ini"
    else
        cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/flash_download.ini" "${OUTPUT}/"
    fi
    change_bin_filename "${OUTPUT}/flash_download.ini" ${BIN_FILENAME}

    LD_FILE_NAME="mt7687_flash.ld"
    if [[ `echo "${BIN_FILENAME}" | grep -c  "bootloader"` -gt 0 ]]; then
        LD_FILE_NAME="bootloader.ld"
    fi

    if [ "${MTK_FOTA_DUAL_IMAGE_ENABLE}" == "y" ]; then
        if [ "${FOTA_PARTITION_B_BUILD}" == "y" ]; then
            LD_FILE_NAME="mt7697_flash_B.ld"
            cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/fota_dual_image/flash_download_B.ini" "${OUTPUT}/flash_download.ini"
            change_bin_filename "${OUTPUT}/flash_download.ini" ${BIN_FILENAME}
        else
            LD_FILE_NAME="mt7697_flash_A.ld"
            cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/fota_dual_image/flash_download_A.ini" "${OUTPUT}/flash_download.ini"
            change_bin_filename "${OUTPUT}/flash_download.ini" ${BIN_FILENAME}
        fi
        echo "${LD_FILE_NAME}"
    fi

    ORIGIN=`grep "XIP_CODE.*(arx)" "${PROJ_DIR}/${LD_FILE_NAME}" | cut -d ',' -f1 | cut -d '=' -f2 | tr -d ' '`
    LENGTH=`grep "XIP_CODE.*(arx)" "${PROJ_DIR}/${LD_FILE_NAME}" | cut -d ',' -f2 | cut -d '/' -f1 | cut -d '=' -f2 | tr -d ' '`
    BEGIN_ADDRESS=`printf "0x%08X" $(($ORIGIN & 0x0FFFFFFF))`
    END_ADDRESS=`printf "0x%08X" $(($BEGIN_ADDRESS + $LENGTH))`
    change_begin_end_address_filename ${OUTPUT}/flash_download.ini ${BEGIN_ADDRESS} $END_ADDRESS


    # copy WIFI fiwmware
    cp -f "${SOURCE_DIR}/prebuilt/driver/chip/mt7687/wifi_n9/WIFI_RAM_CODE_MT76X7_in_flash.bin" "${OUTPUT}/"

    if [ ! -f "${OUTPUT}/${IC_CONFIG}_bootloader.bin" ]; then
        cp -f "${SOURCE_DIR}/project/${BOARD_CONFIG}/apps/bootloader/GCC/bootloader.bin" "${OUTPUT}/${IC_CONFIG}_bootloader.bin"
        echo "${OUTPUT}/${IC_CONFIG}_bootloader.bin doesn't exist. copy default bootloader done."
    fi


elif [ "${BOARD_CONFIG}" == "mt7687_hdk" ]; then
    # copy download config file
    cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/flash_download.ini" "${OUTPUT}/"
    change_bin_filename "${OUTPUT}/flash_download.ini" ${BIN_FILENAME}

    LD_FILE_NAME="mt7687_flash.ld"
    if [[ `echo "${BIN_FILENAME}" | grep -c  "bootloader"` -gt 0 ]]; then
        LD_FILE_NAME="bootloader.ld"
    fi

    if [ "${MTK_FOTA_DUAL_IMAGE_ENABLE}" == "y" ]; then
        if [ "${FOTA_PARTITION_B_BUILD}" == "y" ]; then
            LD_FILE_NAME="mt7687_flash_B.ld"
            cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/fota_dual_image/flash_download_B.ini" "${OUTPUT}/flash_download.ini"
            change_bin_filename "${OUTPUT}/flash_download.ini" ${BIN_FILENAME}
        else
            LD_FILE_NAME="mt7687_flash_A.ld"
            cp -f "${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/fota_dual_image/flash_download_A.ini" "${OUTPUT}/flash_download.ini"
            change_bin_filename ${OUTPUT}/flash_download.ini ${BIN_FILENAME}
        fi
        echo "${LD_FILE_NAME}"
    fi


    ORIGIN=`grep "XIP_CODE.*(arx)" "${PROJ_DIR}/${LD_FILE_NAME}" | cut -d ',' -f1 | cut -d '=' -f2 | tr -d ' '`
    LENGTH=`grep "XIP_CODE.*(arx)" "${PROJ_DIR}/${LD_FILE_NAME}" | cut -d ',' -f2 | cut -d '/' -f1 | cut -d '=' -f2 | tr -d ' '`
    BEGIN_ADDRESS=`printf "0x%08X" $(($ORIGIN & 0x0FFFFFFF))`
    END_ADDRESS=`printf "0x%08X" $((${BEGIN_ADDRESS} + $LENGTH))`
    change_begin_end_address_filename ${OUTPUT}/flash_download.ini ${BEGIN_ADDRESS} ${END_ADDRESS}

    # copy WIFI fiwmware
    cp -f "${SOURCE_DIR}/prebuilt/driver/chip/mt7687/wifi_n9/WIFI_RAM_CODE_MT76X7_in_flash.bin" "${OUTPUT}/"

    # copy bootloader
    if [ ! -f "${OUTPUT}/${IC_CONFIG}_bootloader.bin" ]; then
        cp -f "${SOURCE_DIR}/project/${BOARD_CONFIG}/apps/bootloader/GCC/bootloader.bin" "${OUTPUT}/${IC_CONFIG}_bootloader.bin"
        echo "${OUTPUT}/${IC_CONFIG}_bootloader.bin doesn't exist. copy default bootloader done."
    fi
elif [ "${BOARD_CONFIG}" == "mt7686_hdk" ] || [ "${BOARD_CONFIG}" == "mt7682_hdk" ] || [ "${BOARD_CONFIG}" == "mt5932_hdk" ] || [ "${BOARD_CONFIG}" == "aw7698_evk" ]; then
    # copy prebuilt files
    if [ "${BOARD_CONFIG}" == "aw7698_evk" ]; then
        cp -f "${SOURCE_DIR}/prebuilt/driver/chip/aw7698/mt768x_default_PerRate_TxPwr.bin" "${OUTPUT}/"
        cp -f "${SOURCE_DIR}/prebuilt/driver/chip/aw7698/4K_Buck.bin" "${OUTPUT}/"
    else
        cp -f "${SOURCE_DIR}/prebuilt/driver/chip/mt7686/mt768x_default_PerRate_TxPwr.bin" "${OUTPUT}/"
    fi

    # copy download config file
   FLASH_CONFIG_FULL_PATH="${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/default/${FLASH_CONFIG_FILE-flash_download.cfg}"
   cp -f "$FLASH_CONFIG_FULL_PATH" "${OUTPUT}/flash_download.cfg"

    if [ "${SECURE_BOOT_EN}" == "y" ]; then
        python ${SOURCE_DIR}/tools/secure_boot/scripts/build/append_cfg_bin.py ${OUTPUT}/flash_download.cfg ${SOURCE_DIR}/tools/secure_boot/config/${BOARD_CONFIG}/download/flash_download_secure_boot.cfg ${OUTPUT}/flash_download_sboot.cfg
        cp -f "${OUTPUT}/flash_download_sboot.cfg" "${OUTPUT}/flash_download.cfg"
        rm -f "${OUTPUT}/flash_download_sboot.cfg"
    fi
    # change binary filename in flash configuration
    change_cfg_bin_filename ${OUTPUT}/flash_download.cfg ${BIN_FILENAME}
    if [ -f "${OUTPUT}/wifi_slave_demo.bin" ] && [ "${BOARD_CONFIG}" == "mt5932_hdk" ]; then
       echo "copy ${OUTPUT}/wifi_slave_demo.bin to ${SOURCE_DIR}/prebuilt/driver/chip/mt5932/wifi_image/mt5932_image_sdio.bin"
       cp -f "${OUTPUT}/wifi_slave_demo.bin" "${SOURCE_DIR}/prebuilt/driver/chip/mt5932/wifi_image/mt5932_image_sdio.bin"
    fi

    # copy bootloader
    if [ ! -f "${OUTPUT}/${IC_CONFIG}_bootloader.bin" ]; then
        cp -f "${SOURCE_DIR}/project/${BOARD_CONFIG}/apps/bootloader/GCC/bootloader.bin" "${OUTPUT}/${IC_CONFIG}_bootloader.bin"
        echo "${OUTPUT}/${IC_CONFIG}_bootloader.bin doesn't exist. copy default bootloader done."
    fi

elif [ "${BOARD_CONFIG}" == "ab155x_evk" ] ||
     [ "${BOARD_CONFIG}" == "ab1552_evb" ] || [ "${BOARD_CONFIG}" == "ab1555_evb" ] || [ "${BOARD_CONFIG}" == "ab1556_evb" ] || [ "${BOARD_CONFIG}" == "ab1558_evb" ] ; then
    # copy bt_n9

#    if [ "$MTK_AWS_MCE_ENABLE" == 'y' ]; then
#        # your copy command
#           echo "Copy AWS MCE BT fiwmware bin..."
#           cp -f ${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e1_hdr_aws.bin ${OUTPUT}/ab155x_patch_e1_hdr.bin
#           cp -f ${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e1_hdr_aws_hfp.bin ${OUTPUT}/ab155x_patch_e1_hdr_aws_hfp.bin
#    else
#        # another copy command
#            cp -f ${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e1_hdr.bin ${OUTPUT}/
#    fi

    cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e2_hdr.bin" "${OUTPUT}/ab155x_patch_hdr.bin"
    BOARD_TYPE=${BOARD_TYPE%%_*}_evk
    echo ${BOARD_TYPE}
    # copy flash download cfg
    cp -f "${SOURCE_DIR}/tools/config/$BOARD_TYPE/download/default/flash_download.cfg" "${OUTPUT}/"
    change_cfg_bin_filename ${OUTPUT}/flash_download.cfg ${BIN_FILENAME}

    if [ "$MTK_AWS_MCE_ENABLE" == 'y' ]; then
        if [ "$MTK_BT_SPEAKER_ENABLE" == 'y' ]; then
            echo "Copy AWS MCE Speaker BT E2 fiwmware bin..."
            #cp -f ${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e2_hdr_speaker.bin ${OUTPUT}/ab155x_patch_hdr.bin
            cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e2_hdr_earbud_for_speaker.bin" "${OUTPUT}/ab155x_patch_e2_hdr_earbud_for_speaker.bin"
            cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e2_hdr_headset.bin" "${OUTPUT}/ab155x_patch_e2_hdr_headset.bin"
            cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9_speaker/ab155x_patch_e2_hdr_speaker.bin" "${OUTPUT}/ab155x_patch_e2_hdr_speaker.bin"
            combine_btN9_bins ${OUTPUT}/ab155x_patch_e2_hdr_headset.bin ${OUTPUT}/ab155x_patch_e2_hdr_speaker.bin ${OUTPUT}/ab155x_patch_e2_hdr_earbud_for_speaker.bin ${OUTPUT}/ab155x_patch_hdr.bin
            rm -f "${OUTPUT}/ab155x_patch_e2_hdr_headset.bin"
            rm -f "${OUTPUT}/ab155x_patch_e2_hdr_speaker.bin"
            rm -f "${OUTPUT}/ab155x_patch_e2_hdr_earbud_for_speaker.bin"
        else
            cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e2_hdr_relay.bin" "${OUTPUT}/ab155x_patch_e2_hdr_relay.bin"
            combine_btN9_bins ${OUTPUT}/ab155x_patch_hdr.bin ${OUTPUT}/ab155x_patch_e2_hdr_relay.bin ${OUTPUT}/ab155x_patch_hdr_relay.bin
            rm -f "${OUTPUT}/ab155x_patch_e2_hdr_relay.bin"
            rm -f "${OUTPUT}/ab155x_patch_hdr.bin"
            mv "${OUTPUT}/ab155x_patch_hdr_relay.bin" "${OUTPUT}/ab155x_patch_hdr.bin"
        fi
    fi

    # copy aws hfp flash download cfg
    if [ "$MTK_AWS_MCE_ENABLE" == 'y' ]; then
        cp -f "${SOURCE_DIR}/tools/config/$BOARD_TYPE/download/default/flash_download.cfg" "${OUTPUT}/flash_download_hfp.cfg"
        change_cfg_bin_filename ${OUTPUT}/flash_download_hfp.cfg ${BIN_FILENAME}
        change_cfg_hfp_filename ${OUTPUT}/flash_download_hfp.cfg ${BIN_FILENAME}
    else
        cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e2_hdr_headset.bin" "${OUTPUT}/ab155x_patch_hdr.bin"
    fi
elif [ "${BOARD_CONFIG}" == "am255x_evk" ] || [ "${BOARD_CONFIG}" == "am2552_evk" ]; then

    # add copy 255x E2 patch
    cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e2_hdr.bin" "${OUTPUT}/am255x_patch_hdr.bin"
    BOARD_TYPE=${BOARD_TYPE%%_*}_evk
    echo ${BOARD_TYPE}
    # copy flash download cfg
    cp -f "${SOURCE_DIR}/tools/config/$BOARD_TYPE/download/default/flash_download.cfg" "${OUTPUT}/"
    change_cfg_bin_filename ${OUTPUT}/flash_download.cfg ${BIN_FILENAME}

    # copy aws hfp flash download cfg
    if [ "$MTK_AWS_MCE_ENABLE" == 'y' ]; then
        cp -f "${SOURCE_DIR}/tools/config/$BOARD_TYPE/download/default/flash_download.cfg" "${OUTPUT}/flash_download_hfp.cfg"
        change_cfg_bin_filename ${OUTPUT}/flash_download_hfp.cfg ${BIN_FILENAME}
        change_cfg_hfp_filename ${OUTPUT}/flash_download_hfp.cfg ${BIN_FILENAME}
    else
        cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/bt_n9/ab155x_patch_e2_hdr_headset.bin" "${OUTPUT}/am255x_patch_hdr.bin"
    fi

elif [ "${BOARD_CONFIG}" == "ag3335a_evb" ] || [ "${BOARD_CONFIG}" == "ag3335b_evb" ] || [ "${BOARD_CONFIG}" == "ag3335m_evb" ] || [ "${BOARD_CONFIG}" == "ag3335a_evk" ] || [ "${BOARD_CONFIG}" == "ag3335m_evk" ] || [ "${BOARD_CONFIG}" == "ag3335s_evb" ] || [ "${BOARD_CONFIG}" == "ag3335s_evk" ]; then
    # copy flash download cfg
    if [ "$BOARD" == 'ag3335_fpga' ]; then
    FLASH_CONFIG_FULL_PATH="${SOURCE_DIR}/tools/config/ag3335a_evb/download/default/${FLASH_CONFIG_FILE-flash_download.cfg}"
    cp -f "${FLASH_CONFIG_FULL_PATH}" "${OUTPUT}/flash_download.cfg"
    #cp ${SOURCE_DIR}/tools/config/ag3335a_evb/download/default/ag3335_bootloader.bin ${OUTPUT}/
    else
    FLASH_CONFIG_FULL_PATH="${SOURCE_DIR}/tools/config/${BOARD_CONFIG}/download/default/${FLASH_CONFIG_FILE-flash_download.cfg}"
    cp -f "${FLASH_CONFIG_FULL_PATH}" "${OUTPUT}/flash_download.cfg"
    #cp ${SOURCE_DIR}/tools/config/$BOARD/download/default/flash_download.cfg ${OUTPUT}/
    #cp ${SOURCE_DIR}/tools/config/$BOARD/download/default/ag3335_bootloader.bin ${OUTPUT}/
        if [ "${MTK_GNSS_RTK_ENABLE}" == 'y' ]; then
            cp "${SOURCE_DIR}/prebuilt/middleware/MTK/mnl/rtk/configuration/gnss_config.bin" "${OUTPUT}/gnss_config.bin"
        elif [ "${MTK_GNSS_TIMING_FEATURE_ENABLE}" == 'y' ]; then
            cp "${SOURCE_DIR}/prebuilt/middleware/MTK/mnl/timing/configuration/gnss_config.bin" "${OUTPUT}/gnss_config.bin"
        elif [ "${MTK_GNSS_NAVIC_ENABLE}" == 'y' ]; then
            cp "${SOURCE_DIR}/prebuilt/middleware/MTK/mnl/navic/configuration/gnss_config.bin" "${OUTPUT}/gnss_config.bin"
        elif [ "$MTK_GNSS_L5_ENABLE" == 'n' ]; then
            cp "${SOURCE_DIR}/prebuilt/middleware/MTK/mnl/common/configuration/gnss_config_l1.bin" "${OUTPUT}/gnss_config.bin"
        else
            cp "${SOURCE_DIR}/prebuilt/middleware/MTK/mnl/common/configuration/gnss_config.bin" "${OUTPUT}/gnss_config.bin"
        fi
    fi

    if [ "${SECURE_BOOT_EN}" == "y" ]; then
        echo "secure boot ..."
        python ${SOURCE_DIR}/tools/security/secure_boot/scripts/build/append_cfg_bin.py ${OUTPUT}/flash_download.cfg ${SOURCE_DIR}/tools/security/secure_boot/config/ag3335/download/flash_download_secure_boot.cfg ${OUTPUT}/flash_download_sboot.cfg
        cp -f "${OUTPUT}/flash_download_sboot.cfg" "${OUTPUT}/flash_download.cfg"
        rm -f "${OUTPUT}/flash_download_sboot.cfg"

    fi
    change_cfg_bin_filename ${OUTPUT}/flash_download.cfg ${BIN_FILENAME}

elif [ "${BOARD_CONFIG}" == "mt2625_evb" ]; then
    if [ "${MTK_GNSS_ENABLE}" == 'y' ]; then
        if [ ! -f "${SOURCE_DIR}/prebuilt/middleware/MTK/gnss/firmware/gnss_chip_fw.bin" ]; then
            cp -f "${SOURCE_DIR}/tools/config/$BOARD/download/default/flash_download.cfg" "${OUTPUT}/"
        else
            cp -f "${SOURCE_DIR}/tools/config/$BOARD/download/gnss/flash_download.cfg" "${OUTPUT}/"
            cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/gnss/firmware/gnss_chip_fw.bin" "${OUTPUT}/gnss_firmware.bin"
        fi
    else
        cp -f "${SOURCE_DIR}/tools/config/$BOARD/download/default/flash_download.cfg" "${OUTPUT}/"
    fi
    if [ ! -d "${SOURCE_DIR}/middleware/MTK/nbiot" ]; then
        cp -f "${SOURCE_DIR}/prebuilt/middleware/MTK/nbiot/db/*.dec" "${OUTPUT}/"
    fi
    change_cfg_bin_filename ${OUTPUT}/flash_download.cfg ${BIN_FILENAME}
elif [ "${BOARD_CONFIG}" == "mt2822x_evb" ] || [ "${BOARD_CONFIG}" == "mt2822x_evk" ]; then

    # copy flash download cfg.
    # If FLASH_CONFIG_FILE is defined in the feature.mk file,
    # then it will be used as the downloaded configuration file later.
    # Otherwise, use the default flash_download.cfg file.

    # The flash_download.cfg will be automatically generated in the future, and these script commands will be kept here for now.
    # FLASH_CFG_FULL_PATH="${SOURCE_DIR}/tools/config/mt2822/download/default/${FLASH_CONFIG_FILE-flash_download.cfg}"
    # cp -f "${FLASH_CFG_FULL_PATH}" "${OUTPUT}/flash_download.cfg"

    if [[ "${BIN_FILENAME}" =~ "bootloader" ]] ;then
        sed -i "s|bootloader.bin|${BIN_FILENAME}|g" ${OUTPUT}/flash_download.cfg
        cp ${OUTPUT}/flash_download.cfg ${OUTPUT}/modified_in_BL_flash_download.cfg.debug
    else
        change_cfg_bin_filename ${OUTPUT}/flash_download.cfg ${BIN_FILENAME}

        # comment lm_ama if lm_ama.bin not exist
        if [ ! -e "${OUTPUT}/lm_ama.bin" ]; then
            sed -i -n '{/rom:/{x;n;{/lm_ama/{x;s|^|#|;p;x;tc};x;p;be}};{:c;/\blm_ama/,+2s|^|#|;}};p;bend;:e;x;p;:end' ${OUTPUT}/flash_download.cfg
        fi
        # comment lm_gva if lm_gva.bin not exist
        if [ ! -e "${OUTPUT}/lm_gva.bin" ]; then
            sed -i -n '{/rom:/{x;n;{/lm_gva/{x;s|^|#|;p;x;tc};x;p;be}};{:c;/\blm_gva/,+2s|^|#|;}};p;bend;:e;x;p;:end' ${OUTPUT}/flash_download.cfg
        fi
    fi

    if [ "${SECURE_BOOT_EN}" == "y" ]; then
        echo "secure boot ..."
        cp -f "${OUTPUT}/flash_download.cfg" "${OUTPUT}/flash_download_rsa.cfg"
        cp -f "${OUTPUT}/flash_download.cfg" "${OUTPUT}/flash_download_cmac.cfg"
        ./${SOURCE_DIR}/tools/security/secure_boot/scripts/build/append_dl_cfg.sh ${OUTPUT}/flash_download_rsa.cfg rsa ${BOARD_CONFIG} ${IC_CONFIG}
        ./${SOURCE_DIR}/tools/security/secure_boot/scripts/build/append_dl_cfg.sh ${OUTPUT}/flash_download_cmac.cfg cmac ${BOARD_CONFIG} ${IC_CONFIG}
    fi
elif [ "${BOARD_CONFIG}" == "ab156x_evb" ] || [ "${BOARD_CONFIG}" == "ab156x_evk" ]; then
    echo ${BOARD_TYPE}
    # copy flash download cfg
    # if [ "$BOARD_TYPE" == "ab1568_evb" ] || [ "$BOARD_TYPE" == "ab1568_evk" ] || [ "$BOARD_TYPE" == "ab1568_earbuds" ]; then
    #     cp "${SOURCE_DIR}/tools/config/ab1568_evb/download/default/flash_download.cfg" "${OUTPUT}/"
    # elif [ "$BOARD_TYPE" == "ab1565_cell" ]; then
    #     cp "${SOURCE_DIR}/tools/config/$BOARD_TYPE/download/default/flash_download.cfg" "${OUTPUT}/"
    # elif [ "$BOARD_TYPE" == "ab1565_8m_cell" ]; then
    #     cp "${SOURCE_DIR}/tools/config/ab1565_evb/download/default/flash_download_8m.cfg" "${OUTPUT}/flash_download.cfg"
    # else
    #     cp "${SOURCE_DIR}/tools/config/ab1565_evb/download/default/flash_download.cfg" "${OUTPUT}/"
    # fi

    if [[ "${BIN_FILENAME}" =~ "bootloader" ]] ;then
        sed -i "s|bootloader.bin|${BIN_FILENAME}|g" ${OUTPUT}/flash_download.cfg
        cp ${OUTPUT}/flash_download.cfg ${OUTPUT}/modified_in_BL_flash_download.cfg.debug
    else
        change_cfg_bin_filename ${OUTPUT}/flash_download.cfg ${BIN_FILENAME}

        # comment lm_gva if lm.bin not exist
        if [ ! -e "${OUTPUT}/lm.bin" ]; then
            sed -i -n '{/rom:/{x;n;{/lm/{x;s|^|#|;p;x;tc};x;p;be}};{:c;/\blm/,+2s|^|#|;}};p;bend;:e;x;p;:end' ${OUTPUT}/flash_download.cfg
        fi
    fi

    if [ "$SECURE_BOOT_EN" == "y" ]; then
        echo "secure boot ..."
        cp -f "${OUTPUT}/flash_download.cfg" "${OUTPUT}/flash_download_rsa.cfg"
        cp -f "${OUTPUT}/flash_download.cfg" "${OUTPUT}/flash_download_cmac.cfg"
        ./${SOURCE_DIR}/tools/security/secure_boot/scripts/build/append_dl_cfg.sh ${OUTPUT}/flash_download_rsa.cfg rsa ${BOARD_CONFIG} ${IC_CONFIG}
        ./${SOURCE_DIR}/tools/security/secure_boot/scripts/build/append_dl_cfg.sh ${OUTPUT}/flash_download_cmac.cfg cmac ${BOARD_CONFIG} ${IC_CONFIG}
    fi
elif [ "$BOARD_CONFIG" == "mt2833x_fpga" ]; then
    echo ${BOARD_TYPE}
    #workaround for language model, will be removed when LM management is ready
    #test -e $SOURCE_DIR/middleware/MTK/audio/record_control/inc/WR_50k.en-US.alexa.bin && cp -f $SOURCE_DIR/middleware/MTK/audio/record_control/inc/WR_50k.en-US.alexa.bin $OUTPUT/
    #test -e $SOURCE_DIR/middleware/MTK/audio/record_control/inc/cmn_hans_new.ota.bin && cp -f $SOURCE_DIR/middleware/MTK/audio/record_control/inc/cmn_hans_new.ota.bin $OUTPUT/
    #test -e $SOURCE_DIR/middleware/MTK/audio/record_control/inc/LM_GSound_AMA.bin && cp -f $SOURCE_DIR/middleware/MTK/audio/record_control/inc/LM_GSound_AMA.bin $OUTPUT/
    # copy flash download cfg
    # cp $SOURCE_DIR/tools/config/mt2822/download/default/flash_download.cfg $OUTPUT/
	# if [ "$SECURE_BOOT_EN" == "y" ]; then
    #     echo "secure boot ..."
    #     cp -f $OUTPUT/flash_download.cfg $OUTPUT/flash_download_rsa.cfg
    #     cp -f $OUTPUT/flash_download.cfg $OUTPUT/flash_download_cmac.cfg
    #     ./$SOURCE_DIR/tools/security/secure_boot/scripts/build/append_dl_cfg.sh $OUTPUT/flash_download_rsa.cfg rsa $BOARD_CONFIG $IC_CONFIG
    #     ./$SOURCE_DIR/tools/security/secure_boot/scripts/build/append_dl_cfg.sh $OUTPUT/flash_download_cmac.cfg cmac $BOARD_CONFIG $IC_CONFIG
    # fi
    echo "Copy security boot image ..."
    if [ "$MTK_BOOT_TARGET" == "FLASH" ]; then
        echo ${MTK_BOOT_TARGET}, " copy flash fw"
        change_cfg_bin_filename $OUTPUT/flash_download.cfg $BIN_FILENAME
        sed -i "s|0x18|0x08|g" $OUTPUT/flash_download.cfg
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/mt2833_evb_no_rtos_s.cmm $OUTPUT/mt2833_evb_no_rtos_s.cmm
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/secure_no_rtos_demo.elf  $OUTPUT/secure_no_rtos_demo.elf
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/secure_no_rtos_demo.bin  $OUTPUT/secure_no_rtos_demo.bin
    elif [ "$MTK_BOOT_TARGET" == "SYSRAM" ]; then
        echo ${MTK_BOOT_TARGET}" copy sysram fw"
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/mt2833_evb_no_rtos_s.cmm $OUTPUT/mt2833_evb_no_rtos_s.cmm
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/secure_no_rtos_demo.elf  $OUTPUT/secure_no_rtos_demo.elf
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/secure_no_rtos_demo.bin  $OUTPUT/secure_no_rtos_demo.bin
    else
        echo "This is security build, nothing to do"
    fi
elif [ "$BOARD_CONFIG" == "mt2833x_evb" ]; then
    echo ${BOARD_TYPE}
    echo "Copy security boot image ..."
    if [ "$MTK_BOOT_TARGET" == "FLASH" ]; then
        echo ${MTK_BOOT_TARGET}, " copy flash fw"
        change_cfg_bin_filename $OUTPUT/flash_download.cfg $BIN_FILENAME
        sed -i "s|0x18|0x08|g" $OUTPUT/flash_download.cfg
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/mt2833_evb_no_rtos_s.cmm $OUTPUT/mt2833_evb_no_rtos_s.cmm
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/secure_no_rtos_demo.elf  $OUTPUT/secure_no_rtos_demo.elf
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/secure_no_rtos_demo.bin  $OUTPUT/secure_no_rtos_demo.bin
    elif [ "$MTK_BOOT_TARGET" == "SYSRAM" ]; then
        echo ${MTK_BOOT_TARGET}" copy sysram fw"
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/mt2833_evb_no_rtos_s.cmm $OUTPUT/mt2833_evb_no_rtos_s.cmm
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/secure_no_rtos_demo.elf  $OUTPUT/secure_no_rtos_demo.elf
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/secure_no_rtos_demo.bin  $OUTPUT/secure_no_rtos_demo.bin
    else
        echo "This is security build, nothing to do"
    fi
elif [ "$BOARD_CONFIG" == "mt2833x_evk" ]; then
    echo ${BOARD_TYPE}
    echo "Copy security boot image ..."
    if [ "$MTK_BOOT_TARGET" == "FLASH" ]; then
        echo ${MTK_BOOT_TARGET}, " copy flash fw"
        change_cfg_bin_filename $OUTPUT/flash_download.cfg $BIN_FILENAME
        sed -i "s|0x18|0x08|g" $OUTPUT/flash_download.cfg
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/mt2833_evb_no_rtos_s.cmm $OUTPUT/mt2833_evb_no_rtos_s.cmm
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/secure_no_rtos_demo.elf  $OUTPUT/secure_no_rtos_demo.elf
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw_rom/secure_no_rtos_demo.bin  $OUTPUT/secure_no_rtos_demo.bin
    elif [ "$MTK_BOOT_TARGET" == "SYSRAM" ]; then
        echo ${MTK_BOOT_TARGET}" copy sysram fw"
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/mt2833_evb_no_rtos_s.cmm $OUTPUT/mt2833_evb_no_rtos_s.cmm
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/secure_no_rtos_demo.elf  $OUTPUT/secure_no_rtos_demo.elf
        cp -f $SOURCE_DIR/prebuilt/middleware/MTK/security_fw/secure_no_rtos_demo.bin  $OUTPUT/secure_no_rtos_demo.bin
    else
        echo "This is security build, nothing to do"
    fi
fi


# merge external dsp bin
if [ "${IC_CONFIG}" == "mt2533" ]; then
    if [[ `echo "${BIN_FILENAME}" | grep -c  "bootloader"` -eq 0 ]]; then
        # use test bin file
        if [ "${MTK_BUILD_SMT_LOAD}" == "y" ]; then
            echo "${SOURCE_DIR}/driver/board/component/external_dsp/config_test/merge.sh external_dsp.bin ${OUTPUT}"
            ./${SOURCE_DIR}/driver/board/component/external_dsp/config_test/merge.sh external_dsp.bin ${OUTPUT}
        elif [ "${MTK_AUTOMOTIVE_SUPPORT}" == "y" ]; then
            echo "${SOURCE_DIR}/driver/board/component/external_dsp/config_auto/merge.sh external_dsp.bin ${OUTPUT}"
            ./${SOURCE_DIR}/driver/board/component/external_dsp/config_auto/merge.sh external_dsp.bin ${OUTPUT}
        elif [ "${MTK_NDVC_ENABLE}" == "y" ]; then
            echo "${SOURCE_DIR}/driver/board/component/external_dsp/config_ndvc/merge.sh external_dsp.bin ${OUTPUT}"
            ./${SOURCE_DIR}/driver/board/component/external_dsp/config_ndvc/merge.sh external_dsp.bin ${OUTPUT}
        else
            echo "${SOURCE_DIR}/driver/board/component/external_dsp/config/merge.sh external_dsp.bin ${OUTPUT}"
            ./${SOURCE_DIR}/driver/board/component/external_dsp/config/merge.sh external_dsp.bin ${OUTPUT}
        fi
    fi
fi

# copy .cmm file
for i in ${PROJ_DIR}/*.cmm ; do
        [ -e "$i" ] || continue
        echo "cp $i to ${OUTPUT}/"
        cp -f "${i}" "${OUTPUT}/"
done

exit 0
