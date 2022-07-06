#!/bin/bash

ld_file=$1
output_dir=$2
board_type=$3
project=$4


<<'COMMENT'
sed 'expression_a' | sed 'expression_b' == sed 'expression_a;expression_b'
entrys=`cat $ld_file | grep DOWNLOAD | grep name | grep display | sed 's/[ \t\{\}\/\*\\]//g;s/DOWNLOAD//g'`
echo "$entrys"

The GNU linker says:
    The keyword ORIGIN may be abbreviated to org or o (but not, for example, ORG).
    The keyword LENGTH may be abbreviated to len or l.
COMMENT

# append_download_entry    $1      $2               $3           $4
# append_download_entry    file    start_address    file_path    display
function append_download_entry(){
    echo '        - rom:' >> $1
    echo '            file: '$3 >> $1
    echo '            name: '$4 >> $1
    echo '            begin_address: '$2 >> $1
}


# add_general_setting    $1      $2          $3
# add_general_setting    file    platform    cfg_version
function add_general_setting(){
    echo '###############################################################################################' >> $1
    echo '#' >> $1
    echo '#  General Setting' >> $1
    echo '#' >> $1
    echo '###############################################################################################' >> $1
    echo 'general:' >> $1
    echo '    platform: '$2 >> $1
    echo '    config_version: '$3 >> $1
    echo '' >> $1
    echo '' >> $1
}


# add_main_region_setting    $1      $2
# add_main_region_setting    file    address_type
function add_main_region_setting(){
    echo '###############################################################################################' >> $1
    echo '#' >> $1
    echo '#  Main Region Setting' >> $1
    echo '#' >> $1
    echo '###############################################################################################' >> $1
    echo 'main_region:' >> $1
    echo '    address_type: '$2 >> $1
    echo '    rom_list:' >> $1
}


<<'COMMENT'
    command example:
        ./auto_download_cfg.sh    <chip_name>_sysram.ld    .    <chip_name> BL
        ./auto_download_cfg.sh    <chip_name>_flash.ld     .    <chip_name> MCU_FW
COMMENT

cfg_file_path=$output_dir/flash_download.cfg
if [[ "$project" == "BL" ]] || [[ ! -f $cfg_file_path ]]; then
    rm -f $cfg_file_path
    touch $cfg_file_path
    add_general_setting $cfg_file_path $board_type v2.0
    add_main_region_setting $cfg_file_path physical
    for line in `cat $ld_file | grep DOWNLOAD | grep name | grep display | sed 's/[ \t\{\}\/\*\\]//g;s/DOWNLOAD//g'`; do
        # echo "BL project or flash_download.cfg file not exist, $line"
        reg_exp=':(ORIGIN|org|o)=(.*),(LENGTH|len|l)=(.*),name:(.*),display:(.*)'
        [[ "$line" =~ $reg_exp ]]
        append_download_entry $cfg_file_path "${BASH_REMATCH[2]}" "${BASH_REMATCH[5]}" "${BASH_REMATCH[6]}"
    done
else
    for line in `cat $ld_file | grep DOWNLOAD | grep name | grep display | sed 's/[ \t\{\}\/\*\\]//g;s/DOWNLOAD//g'`; do
        # echo "not BL, just append download entry $line"
        reg_exp=':(ORIGIN|org|o)=(.*),(LENGTH|len|l)=(.*),name:(.*),display:(.*)'
        [[ "$line" =~ $reg_exp ]]
        if [ "${BASH_REMATCH[6]}" != "BootLoader" ]; then
            append_download_entry $cfg_file_path "${BASH_REMATCH[2]}" "${BASH_REMATCH[5]}" "${BASH_REMATCH[6]}"
        fi
    done
fi