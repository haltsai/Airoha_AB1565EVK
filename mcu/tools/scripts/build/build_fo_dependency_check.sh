#!/bin/bash

# This script will start the compilation of cm4/dsp according to the configuration file (mapping_proj.cfg).
# Build cm4 would bring bl option by default.

###############################################################################
# Variables
OUT="${PWD}/out"
working_dir="${PWD}"
debug="0"
feature_mk=""
feature_mk_dsp0=""
feature_mk_dsp1=""
default_dsp_bin_switch="off"
compatibleTest="off"
pwdLength=""
pwd_folder_length_limit=23
out_folder_length_limit=65
COLOR_RED='\e[0;31m'
COLOR_REST='\e[0m'
subst_disk=
mapping_disk_file="mapping_disk"
current_subst_info_file="current_subst_info"
co_build_command=$(echo "$0 $@")
dsp_opt_file=""
mcu_opt_file=""

umount_subst_disk () {
    subst ${subst_disk}: //D
    echo "Disk ${subst_disk}: mapping clear"
}

subst_disk () {
    if [ -f ${mapping_disk_file} ]; then
        subst > $current_subst_info_file
        subst_disk=$(head -1 ${mapping_disk_file} | tr -d '\n' | tr -d '\r')
        working_dir_disk=$(echo ${working_dir:1:2} | tr -d '\n' | tr -d '\r' | tr -d '\/')
        subst_dir=$(tail -1 ${mapping_disk_file} | tr -d '\n' | tr -d '\r' )
        subst_dir_without_slash=$(tail -1 ${mapping_disk_file} | tr -d '\/' | tr -d '\n' | tr -d '\r' )
        echo "Mapping sdk folder to disk ${subst_disk}:"
        echo "\$subst_disk : ${subst_disk}"
        echo "\$subst_dir : ${subst_dir}"
        echo "\$subst_dir_without_slash : ${subst_dir_without_slash}"
        echo "\$working_dir_disk : ${working_dir_disk}"

        # codebase folder name changed , in the same partition or not
        if [ "${subst_disk}" != "${working_dir_disk}" ]; then
            # umount susbst when folder name changed
            if [[ "$working_dir" != "${subst_dir}" ]]; then
                echo "${subst_disk}" > "${mapping_disk_file}"
                echo "${working_dir}" >> "${mapping_disk_file}"
                umount_subst_disk
            fi
            # use the same subst as before
            subst ${subst_disk}: . 2>&1 > /dev/null
            working_dir=${subst_disk}:
        fi
        
        if [ -f ${current_subst_info_file} ]; then
            # check subst_disk used by another path or not
            while read line; do
                # reading each line
                #echo $line
                # split by space, [0]:disk  [1]: =>  [2]:  path
                stringarray=$line
                current_subst_disk=$(echo $line | cut -d' ' -f1)
                current_dir=$(echo $line | cut -d' ' -f3 | tr -d ':' | tr -d '\n' | tr -d '\r')
                # disk used
                if [[ "${current_subst_disk:0:1}" == ${subst_disk^^} ]]; then
                    # mount path is different
                    if [[ "${current_dir,,}" != ${subst_dir_without_slash,,} ]]; then
                        echo "\$stringarray : ${stringarray}"
                        echo "\$current_subst_disk : ${current_subst_disk:0:1}"
                        echo "\$current_dir : ${current_dir}"
                        echo "\$subst_dir_without_slash : ${subst_dir_without_slash}"
                        working_dir="${PWD}"
                        subst_disk_dynamically
                        #echo "run function subst_disk_dynamically"
                        break
                    fi
                fi
            done < ${current_subst_info_file}
            rm $current_subst_info_file
        fi
        
    else
        subst_disk_dynamically
    fi
}

subst_disk_dynamically () {
    # dynamic
    for i in {H..Z}
    do
        subst $i: . 2>&1 > /dev/null
        if [[ $? -eq 0 ]]; then
            echo "$i" > "${mapping_disk_file}"
            echo "${working_dir}" >> "${mapping_disk_file}"
            echo "Mapping sdk folder to disk $i:"
            working_dir=$i:
            break
        fi
    done
}

platform=$(uname)
if [[ "${platform}" =~ "MINGW" || "${platform}" =~ "MSYS" ]]; then
    # not include forward slash
    pwdLength=$(echo ${PWD} | wc -m)
    subst_disk
elif [[ "${platform}" =~ "Darwin" ]]; then
    export PATH="/usr/local/bin:${PATH}"
fi

# check /bin/bash
if [ -f /bin/bash ]; then
    export SHELL="/bin/bash"
fi

#source mapping_proj.cfg
source mcu/tools/mapping_proj/mapping_proj.cfg

source mcu/tools/mapping_proj/usage_function

show_available_proj () {
    echo "==============================================================="
    echo "Available Build Projects:"
    echo "==============================================================="
    for var in ${!map_@}
    do
        board=$(echo ${var} | sed 's/__/:/g' | cut -d':' -f2)
        project=$(echo ${var} | sed 's/__/:/g' | cut -d':' -f3)
        echo "${board}"
        echo "  ${project}"
        eval proj_map_key=( \${!$var[@]} )
        eval proj_map_value=( \${$var[@]} )
        for key in ${proj_map_key[@]}
        do
            displayName=""
            case ${key} in
                0)
                 displayName="board_share"
                    ;;
                1)
                 displayName="cm4"
                    ;;
                2)
                 displayName="dsp0"
                    ;;
                3)
                 displayName="dsp1"
                    ;;
                4)
                 displayName="fm"
                    ;;
                5)
                 displayName="fd0"
                    ;;
                6)
                 displayName="fd1"
                    ;;
            esac
            eval echo \"\ \ \ \ \ \ ${displayName}: \${${var}[\"${key}\"]}\"
        done
    done
}

clean_out () {
    rm -rf "${1}"
    echo "rm -rf ${1}"
}

check_folder_length () {
    #echo -e "\nout_folder_length_limit=$out_folder_length_limit"
    pwdLength=${1}
    out_folder_length=$(echo ${2}| wc -m)
    if [[ "${platform}" =~ "MINGW" || "${platform}" =~ "MSYS" ]]; then
        if [ "$(( pwdLength + out_folder_length - 1 ))" -gt "$(( pwd_folder_length_limit + out_folder_length_limit ))" ]; then

            echo -e "\n Error: over maximum path length"
            if [ "$(( pwdLength - 1 ))" -gt "${pwd_folder_length_limit}" ]; then
                echo -e "    1. Please install SDK in a path less than ${pwd_folder_length_limit} characters"
            fi

            if [ "${out_folder_length}" -gt "${out_folder_length_limit}" ]; then
                echo -e "    2. Please shorten the name of project or feature."
            fi

            exit 1
        else
            return 0
        fi
    fi
    return 0
}

# copy cm4 files
copy_cm4_files () {
    debug_dir=""
    download_dir=""
    if [  ${debug} -eq "1" ]; then
        echo "1 : ${1}"
        echo "2 : ${2}"
        echo "3 : ${3}"
        echo "4 : ${4}"
        echo "5 : ${5}"
    fi
    if [ "${4}" == "none" ]; then
        debug_dir=out/${1}/${3}/debug
        download_dir=out/${1}/${3}/download
    else
        debug_dir=out/${4}/${3}/debug
        download_dir=out/${4}/${3}/download
    fi
    if [ ! -e "${debug_dir}" ]; then
        mkdir -p "${debug_dir}"
    fi
    if [ ! -e "${download_dir}" ]; then
        mkdir -p "${download_dir}"
    fi
    # copy debug files if exist
    echo "cp -f mcu/out/${1}/${2}/${5}/${2}.elf  ${debug_dir}"
    echo "cp -f mcu/out/${1}/${2}/${5}/${2}.dis  ${debug_dir}"
    echo "cp -f mcu/out/${1}/${2}/${5}/${2}.map  ${debug_dir}"
    echo "cp -f mcu/out/${1}/${2}/${5}/cm4_log_str.bin  ${debug_dir}"
    test -e "mcu/out/${1}/${2}/${5}/${2}.elf" && cp -f "mcu/out/${1}/${2}/${5}/${2}.elf"  "${debug_dir}"
    test -e "mcu/out/${1}/${2}/${5}/${2}.dis" && cp -f "mcu/out/${1}/${2}/${5}/${2}.dis"  "${debug_dir}"
    test -e "mcu/out/${1}/${2}/${5}/${2}.map" && cp -f "mcu/out/${1}/${2}/${5}/${2}.map"  "${debug_dir}"
    test -e "mcu/out/${1}/${2}/${5}/cm4_log_str.bin" && cp -f "mcu/out/${1}/${2}/${5}/cm4_log_str.bin"  "${debug_dir}"

    # copy download files if exist
    echo "cp -f mcu/out/$1/$2/$5/flash_download.cfg $download_dir"
    echo "cp -f mcu/out/$1/$2/$5/*_bootloader.bin $download_dir"
    echo "cp -f mcu/out/$1/$2/$5/$2.bin $download_dir"
    echo "cp -f mcu/out/$1/$2/$5/partition_table.bin $download_dir"
    echo "cp -f mcu/out/$1/$2/$5/*_patch_hdr.bin $download_dir"
    echo "cp -f mcu/out/$1/$2/$5/nvdm.bin $download_dir"
    echo "cp -f mcu/out/$1/$2/$5/filesystem.bin $download_dir"
    test -e "mcu/out/$1/$2/$5/flash_download.cfg" && cp -f "mcu/out/$1/$2/$5/flash_download.cfg" "${download_dir}"
    test -e "mcu/out/$1/$2/$5/"*_bootloader.bin && cp -f "mcu/out/$1/$2/$5/"*_bootloader.bin "${download_dir}"
    test -e "mcu/out/$1/$2/$5/$2.bin" && cp -f "mcu/out/$1/$2/$5/$2.bin" "${download_dir}"
    test -e "mcu/out/$1/$2/$5/partition_table.bin" && cp -f "mcu/out/$1/$2/$5/partition_table.bin" "${download_dir}"
    test -e "mcu/out/$1/$2/$5/"*_patch_hdr.bin && cp -f "mcu/out/$1/$2/$5/"*_patch_hdr.bin "${download_dir}"
    test -e "mcu/out/$1/$2/$5/nvdm.bin" && cp -f "mcu/out/$1/$2/$5/nvdm.bin" "${download_dir}"
    test -e "mcu/out/$1/$2/$5/filesystem.bin" && cp -f "mcu/out/$1/$2/$5/"filesystem.bin "${download_dir}"


    # Copy the language model binary to the output folder.
    # For gva and ama are in different partition
    if [ -e "mcu/out/${1}/${2}/${5}/lm_gva.bin" ] ; then
        echo "cp -f mcu/out/${1}/${2}/${5}/lm_gva.bin ${download_dir}"
        cp -f "mcu/out/${1}/${2}/${5}/lm_gva.bin" "${download_dir}"
    fi
    if [ -e "mcu/out/${1}/${2}/${5}/lm_ama.bin" ] ; then
        echo "cp -f mcu/out/${1}/${2}/${5}/lm_ama.bin ${download_dir}"
        cp -f "mcu/out/${1}/${2}/${5}/lm_ama.bin" "${download_dir}"
    fi
    # For gva and ama are in the same partition
    if [ -e "mcu/out/${1}/${2}/${5}/lm.bin" ] ; then
        echo "cp -f mcu/out/${1}/${2}/${5}/lm.bin ${download_dir}"
        cp -f "mcu/out/${1}/${2}/${5}/lm.bin" "${download_dir}"
    fi
}

# copy dsp files
copy_dsp_files () {
    if [  ${debug} -eq "1" ]; then
        echo "1 : ${1}"
        echo "2 : ${2}"
        echo "3 : ${3}"
        echo "4 : ${4}"
        echo "5 : ${5}"
        echo "6 : ${6}"
    fi
    debug_dir=""
    download_dir=""
    if [ "${4}" == "none" ]; then
        debug_dir="out/${1}/${3}/debug"
        download_dir="out/${1}/${3}/download"
    else
        debug_dir="out/${4}/${3}/debug"
        download_dir="out/${4}/${3}/download"
    fi
    if [ ! -e "${debug_dir}" ]; then
        mkdir -p "${debug_dir}"
    fi
    if [ ! -e "${download_dir}" ]; then
        mkdir -p "${download_dir}"
    fi

    # copy debug files if exist
    echo "cp -f ${6}/${1}/${2}/${5}/${2}.out  ${debug_dir}"
    echo "cp -f ${6}/${1}/${2}/${5}/${2}.asm  ${debug_dir}"
    echo "cp -f ${6}/${1}/${2}/${5}/${2}.map  ${debug_dir}"
    echo "cp -f ${6}/${1}/${2}/${5}/dsp*_log_str.bin  ${debug_dir}"
    test -e "${6}/${1}/${2}/${5}/${2}.out" && cp -f "${6}/${1}/${2}/${5}/${2}.out"  "${debug_dir}"
    test -e "${6}/${1}/${2}/${5}/${2}.asm" && cp -f "${6}/${1}/${2}/${5}/${2}.asm"  "${debug_dir}"
    test -e "${6}/${1}/${2}/${5}/${2}.map" && cp -f "${6}/${1}/${2}/${5}/${2}.map"  "${debug_dir}"
    test -e "${6}/${1}/${2}/${5}/"dsp*_log_str.bin && cp -f "${6}/${1}/${2}/${5}/"dsp*_log_str.bin  "${debug_dir}"

    # copy download files if exist
    echo "cp -f ${6}/${1}/${2}/${5}/${2}.bin ${download_dir}"
    if [ -f "${6}/${1}/${2}/${5}/${2}.bin" ]; then
        cp -f "${6}/${1}/${2}/${5}/${2}.bin" "${download_dir}"
    else
        echo -e "${COLOR_RED}File ${6}/${1}/${2}/${5}/${2}.bin not exist!${COLOR_REST}"
    fi

}

###############################################################################
# Parsing arguments
if [ "$#" -eq "0" ]; then
    show_usage
    exit 1
fi

declare -a argv=($0)
for i in $@
do
    case ${i} in
        list)
            if [ "${#argv[@]}" == "3" ]; then
                board=${argv[1]}
                project=${argv[2]}
                mapping_var="map__${board}__${project}"
                eval dsp1_proj=\${$mapping_var[3]}
                eval dsp0_proj=\${$mapping_var[2]}
                eval cm4_proj=\${$mapping_var[1]}
                eval board_share=\${$mapping_var[0]}
                if [ ! -z "${board_share}" ]; then
                    board=${board_share}
                fi
                cd ${working_dir}/mcu
                ./build.sh ${board} ${project} "list"
                cd ${working_dir}
                echo -e "\n\n"
                cd ${working_dir}/dsp
                if [ ! -z "${dsp0_proj}" ]; then
                    ./build.sh ${board} ${dsp0_proj} "list"
                fi
                if [ !  -z "${dsp1_proj}" ]; then
                    ./build.sh ${board} ${dsp1_proj} "list"
                fi
                cd ${working_dir}
                exit 0
            elif [ "${#argv[@]}" == "1" ]; then
                show_available_proj
                exit 0
            else
                #echo "args : ${#argv[@]}"
                echo "Error: wrong usage!!"
                exit 1
            fi
            ;;
        list_board_share)
            if [ "${#argv[@]}" == "3" ]; then
                board=${argv[1]}
                project=${argv[2]}
                mapping_var="map__${board}__${project}"
                eval board_share=\${$mapping_var[0]}
                echo "${board_share}"
                exit 0
            fi
            ;;
        -fm=*)
            feature_mk="${i#*=}"
            ;;
        -fd0=*)
            feature_mk_dsp0="${i#*=}"
            ;;
        -fd1=*)
            feature_mk_dsp1="${i#*=}"
            ;;
        -mcu)
            default_dsp_bin_switch="on"
            ;;
        -compTest)
            compatibleTest="on"
            ;;
        -o=*|--option=*)
            opt="${i#*=}"
            extra_opt+="-o=${opt} "
            shift
            ;;
        -odsp=*|--option_dsp=*)
            opt="${i#*=}"
            extra_opt_dsp+="-o=${opt} "
            shift
            ;;
        -omcu=*|--option_mcu=*)
            opt="${i#*=}"
            extra_opt_mcu+="-o=${opt} "
            shift
            ;;
        -*)
            echo "Error: unknown parameter \"$i\""
            show_usage
            exit 1
            ;;
        *)
            argv+=($i)
            ;;
    esac
done

# check configurations
board=${argv[1]}
project=${argv[2]}
fake_board="none"
output_dir=${OUT}/${board}/${project}
mapping_var="map__${board}__${project}"
eval board_share=\${$mapping_var[0]}
eval cm4_proj=\${$mapping_var[1]}
eval dsp0_proj=\${$mapping_var[2]}
eval dsp1_proj=\${$mapping_var[3]}

if [ ! -z "${board_share}" ]; then
    fake_board=${board}
    board=${board_share}
    echo -e "board from mapping_proj.cfg: ${board}\n"
fi

if [  ${debug} -eq "1" ]; then
    echo -e "feature_mk : ${feature_mk}\n"
    echo -e "feature_mk_dsp0 : ${feature_mk_dsp0}\n"
    echo -e "feature_mk_dsp1 : ${feature_mk_dsp1}\n"
fi

if [ -z "${feature_mk}${feature_mk_dsp0}${feature_mk_dsp1}" ]; then
    eval feature_mk=\${$mapping_var[4]}
    eval feature_mk_dsp0=\${$mapping_var[5]}
    eval feature_mk_dsp1=\${$mapping_var[6]}
fi

if [  ${debug} -eq "1" ]; then
    echo -e "\n\n"
    echo -e "feature_mk from mapping_proj.cfg: ${feature_mk}\n";
    echo -e "feature_mk_dsp0 from mapping_proj.cfg: ${feature_mk_dsp0}\n";
    echo -e "feature_mk_dsp1 from mapping_proj.cfg: ${feature_mk_dsp1}\n";
fi

# To do actions according to arguments
if [ "${argv[3]}" == "clean" ]; then
    if [ "${#argv[@]}" != "4" ]; then
        show_usage
        exit 1
    fi
    if [ ! -z "${dsp1_proj}" ]; then
        clean_out ${working_dir}/dsp/out/${board}/${dsp1_proj}
    fi
    if [ ! -z "${dsp0_proj}" ]; then
        clean_out ${working_dir}/dsp/out/${board}/${dsp0_proj}
    fi
    if [ ! -z "${cm4_proj}" ]; then
        clean_out ${working_dir}/mcu/out/${board}/${cm4_proj}
    fi
    if [ "${fake_board}" == "none" ]; then
        clean_out ${OUT}/${board}/${project}
    else
        clean_out ${OUT}/${fake_board}/${project}
    fi

elif [ "${argv[2]}" == "clean" ]; then
    if [ "${#argv[@]}" != "3" ]; then
        show_usage
        exit 1
    fi
    clean_out ${working_dir}/dsp/out/${board}
    clean_out ${working_dir}/mcu/out/${board}
    if [ "${fake_board}" == "none" ]; then
        clean_out ${OUT}/${board}/${project}
    else
        clean_out ${OUT}/${fake_board}/${project}
    fi
elif [ "${argv[1]}" == "clean" ]; then
    if [ "${#argv[@]}" != "2" ]; then
        show_usage
        exit 1
    fi
    clean_out ${working_dir}/mcu/out
    clean_out ${working_dir}/dsp/out
    clean_out ${OUT}
else
    if [ "${#argv[@]}" != "3" ]; then
        show_usage
        exit 1
    fi
    eval mapping_var_key=\${!$mapping_var[@]}
    if [ -z "${mapping_var_key}" ]; then
        show_usage
        echo "Error: cannot find board=${board} project=${project} configurations in mapping_proj.cfg."
        exit 1
    fi

    rm -rf "${output_dir}/log"
    mkdir -p "${output_dir}/log"
    rm -rf "${output_dir}/debug"
    mkdir -p "${output_dir}/debug"
    rm -rf "${output_dir}/download"
    mkdir -p "${output_dir}/download"
    echo "$co_build_command" > ${output_dir}/log/build_time.log
    echo "Start CO-Build: "`date` >> ${output_dir}/log/build_time.log

    # dsp1 build start
    dsp1_filename="${feature_mk_dsp1%.*}"
    if [ ! -z "${dsp1_proj}" ] && [ "${default_dsp_bin_switch}" == "off" ]; then

        echo "cd ${working_dir}/dsp"
        cd ${working_dir}/dsp
        echo "=============================================================="
        echo "Start DSP1 Build"
        echo "=============================================================="
        #echo "dsp1_filename : ${dsp1_filename}"
        #echo "Start DSP1 Build: "`date` >> $output_dir/log/build_time.log
        if [ ! -z "${feature_mk_dsp1}" ]; then
            ./build.sh ${board} ${dsp1_proj} -f=${feature_mk_dsp1} -o=OUT=out/${board}/${dsp1_proj}/${dsp1_filename} -cobuild ${extra_opt} ${extra_opt_dsp}
            dsp_opt_file="${working_dir}/dsp/out/${board}/${dsp1_proj}/${dsp1_filename}/log/copy_firmware_opts.log"
        else
            ./build.sh ${board} ${dsp1_proj} -f=feature.mk -o=OUT=out/${board}/${dsp1_proj}/feature -cobuild ${extra_opt} ${extra_opt_dsp}
            dsp_opt_file="${working_dir}/dsp/out/${board}/${dsp1_proj}/feature/log/copy_firmware_opts.log"
        fi
        DSP1_BUILD_RESULT=$?
        #echo "End DSP1 Build: "`date` >> $output_dir/log/build_time.log
        echo "=============================================================="
        echo "End DSP1 Build"
        echo "=============================================================="
        # change back to working dir
        echo "cd ${working_dir}/"
        cd ${working_dir}/
        # copy dsp files
        if [ ! -z "${feature_mk_dsp1}" ]; then
            copy_dsp_files ${board} ${dsp1_proj} ${project} ${fake_board} ${dsp1_filename} dsp/out
        else
            copy_dsp_files ${board} ${dsp1_proj} ${project} ${fake_board} feature dsp/out
        fi
    elif [ ! -z "${dsp1_proj}" ] && [ "${default_dsp_bin_switch}" == "on" ]; then
        echo "=============================================================="
        echo "Copy prebuilt DSP1"
        echo "=============================================================="
        # change back to working dir
        echo "cd ${working_dir}/"
        cd ${working_dir}/
        # copy dsp files
        if [ ! -z "${feature_mk_dsp1}" ]; then
            copy_dsp_files ${board} ${dsp1_proj} ${project} ${fake_board} ${dsp1_filename} dsp/prebuilt/dsp1_bin
        else
            copy_dsp_files ${board} ${dsp1_proj} ${project} ${fake_board} feature dsp/prebuilt/dsp1_bin
        fi
        echo "=============================================================="
        echo "End copy prebuilt DSP1"
        echo -e "==============================================================\n"
    fi

    # dsp0 build start
    dsp0_filename="${feature_mk_dsp0%.*}"
    if [ ! -z "${dsp0_proj}" ] && [ "${default_dsp_bin_switch}" == "off" ]; then

        echo "cd ${working_dir}/dsp"
        cd ${working_dir}/dsp
        echo "=============================================================="
        echo "Start DSP0 Build"
        echo "=============================================================="
        #echo "Start DSP0 Build: "`date` >> $output_dir/log/build_time.log
        #echo "dsp0_filename : $dsp0_filename"
        if [ ! -z "${feature_mk_dsp0}" ]; then
            ./build.sh ${board} ${dsp0_proj} -f=${feature_mk_dsp0} -o=OUT=out/${board}/${dsp0_proj}/${dsp0_filename} -cobuild ${extra_opt} ${extra_opt_dsp}
            dsp_opt_file="${working_dir}/dsp/out/${board}/${dsp0_proj}/${dsp0_filename}/log/copy_firmware_opts.log"
        else
            ./build.sh ${board} ${dsp0_proj} -f=feature.mk -o=OUT=out/${board}/${dsp0_proj}/feature -cobuild ${extra_opt} ${extra_opt_dsp}
            dsp_opt_file="${working_dir}/dsp/out/${board}/${dsp0_proj}/feature/log/copy_firmware_opts.log"
        fi
        DSP0_BUILD_RESULT=$?
        #echo "End DSP0 Build: "`date` >> $output_dir/log/build_time.log
        echo "=============================================================="
        echo "End DSP0 Build"
        echo "=============================================================="
        # change back to working dir
        echo "cd ${working_dir}/"
        cd ${working_dir}/
        # copy dsp files
        if [ ! -z "${feature_mk_dsp0}" ]; then
            copy_dsp_files ${board} ${dsp0_proj} ${project} ${fake_board} ${dsp0_filename} dsp/out
        else
            copy_dsp_files ${board} ${dsp0_proj} ${project} ${fake_board} feature dsp/out
        fi
    elif [ ! -z "${dsp0_proj}" ] && [ "${default_dsp_bin_switch}" == "on" ]; then
        echo "=============================================================="
        echo "Copy prebuilt DSP0"
        echo "=============================================================="
        # change back to working dir
        echo "cd ${working_dir}/"
        cd ${working_dir}/
        # copy dsp files
        if [ ! -z "${feature_mk_dsp0}" ]; then
            copy_dsp_files ${board} ${dsp0_proj} ${project} ${fake_board} ${dsp0_filename} dsp/prebuilt/dsp0_bin
        else
            copy_dsp_files ${board} ${dsp0_proj} ${project} ${fake_board} feature dsp/prebuilt/dsp0_bin
        fi
        echo "=============================================================="
        echo "End copy prebuilt DSP0"
        echo -e "==============================================================\n"
    fi

    # cm4 build start
    if [ ! -z "${cm4_proj}" ]; then
        echo "cd ${working_dir}/mcu"
        cd ${working_dir}/mcu
        echo "=============================================================="
        echo "Start CM4 Build"
        echo "=============================================================="
        #echo "Start CM4 Build: "`date` >> $output_dir/log/build_time.log
        cm4_filename="${feature_mk%.*}"
        #echo "cm4_filename : ${cm4_filename}"
        if [ ! -z "${feature_mk}" ]; then
                if [ "$compatibleTest" == "off" ]; then
                    ./build.sh ${board} ${cm4_proj} -f=${feature_mk} -o=OUT=out/${board}/${cm4_proj}/${cm4_filename} bl -cobuild ${extra_opt} ${extra_opt_mcu}
                    mcu_opt_file="${working_dir}/mcu/out/${board}/${cm4_proj}/${cm4_filename}/log/copy_firmware_opts.log"
                else
                    ./build.sh ${board} ${cm4_proj} -f=${feature_mk} -o=OUT=out/${board}/${cm4_proj}/${cm4_filename} bl -compTest ${extra_opt} ${extra_opt_mcu}
                fi
        else
                if [ "$compatibleTest" == "off" ]; then
                    ./build.sh ${board} ${cm4_proj} -f=feature.mk -o=OUT=out/${board}/${cm4_proj}/feature bl -cobuild ${extra_opt} ${extra_opt_mcu}
                    mcu_opt_file="${working_dir}/mcu/out/${board}/${cm4_proj}/feature/log/copy_firmware_opts.log"
                else
                    ./build.sh ${board} ${cm4_proj} -f=feature.mk -o=OUT=out/${board}/${cm4_proj}/feature bl -compTest ${extra_opt} ${extra_opt_mcu}
                fi
        fi
        CM4_BUILD_RESULT=$?
        #echo "End CM4 Build: "`date` >> ${output_dir}/log/build_time.log
        echo "=============================================================="
        echo "End CM4 Build"
        echo -e "==============================================================\n"
        # change back to working dir
        echo "cd ${working_dir}/"
        cd ${working_dir}/
        # copy cm4 files
        if [ ! -z "${feature_mk}" ]; then
            copy_cm4_files ${board} ${cm4_proj} ${project} ${fake_board} ${cm4_filename}
        else
            copy_cm4_files ${board} ${cm4_proj} ${project} ${fake_board} feature
        fi
    fi

    # update flash_download.cfg
    flash_cfg=${output_dir}/download/flash_download.cfg
    if [ -e "${flash_cfg}" ]; then
        if [ ! -z "${dsp0_proj}" ]; then
            sed -i "s|\bdsp0_freertos_create_thread.bin|${dsp0_proj}.bin|g" ${flash_cfg}
            sed -i -n '{/rom:/{x;n;{/dsp0/{x;s|^#||;p;x;tc};x;p;be}};{:c;/\bdsp0_/,+2s|^#||;}};p;bend;:e;x;p;:end' ${flash_cfg}
        fi
        if [ ! -z "${dsp1_proj}" ]; then
            sed -i "s|\bdsp1_no_rtos_initialize_system.bin|${dsp1_proj}.bin|g" ${flash_cfg}
            sed -i -n '{/rom:/{x;n;{/dsp1/{x;s|^#||;p;x;tc};x;p;be}};{:c;/\bdsp1_/,+2s|^#||;}};p;bend;:e;x;p;:end' ${flash_cfg}
        fi
    fi

    # return code
    declare -i BUILD_RESULT=0
    echo "End   CO-Build: "`date` >> ${output_dir}/log/build_time.log
    if [ ! -z "${DSP1_BUILD_RESULT}" ]; then
        if [ "${DSP1_BUILD_RESULT}" -eq "0" ]; then
            echo "DSP1 BUILD: PASS" >> ${output_dir}/log/build_time.log
        else
            echo -e "DSP1 BUILD: ${COLOR_RED}FAIL${COLOR_REST}" >> ${output_dir}/log/build_time.log
            BUILD_RESULT+=1
        fi
    fi
    if [ ! -z "${DSP0_BUILD_RESULT}" ]; then
        if [ "${DSP0_BUILD_RESULT}" -eq "0" ]; then
            echo "DSP0 BUILD: PASS" >> ${output_dir}/log/build_time.log
        else
            echo -e "DSP0 BUILD: ${COLOR_RED}FAIL${COLOR_REST}" >> ${output_dir}/log/build_time.log
            BUILD_RESULT+=1
        fi
    fi
    if [ ! -z "${CM4_BUILD_RESULT}" ]; then
        if [ "${CM4_BUILD_RESULT}" -eq "0" ]; then
            echo "CM4  BUILD: PASS" >> ${output_dir}/log/build_time.log
        else
            echo -e "CM4  BUILD: ${COLOR_RED}FAIL${COLOR_REST}" >> ${output_dir}/log/build_time.log
            BUILD_RESULT+=1
        fi
    fi

    if [ "${BUILD_RESULT}" -eq "0" ]; then
        echo "TOTAL CO-BUILD: PASS (return code ${BUILD_RESULT})" >> ${output_dir}/log/build_time.log
    else
        echo -e "TOTAL CO-BUILD: ${COLOR_RED}FAIL${COLOR_REST} (return code ${BUILD_RESULT})" >> ${output_dir}/log/build_time.log
    fi

    echo "=============================================================="
    echo "Summary CO-BUILD"
    echo "=============================================================="
    cat ${output_dir}/log/build_time.log
    mv -f ${output_dir}/*.log "${output_dir}/log/" 2> /dev/null
    echo -e "\n\n"
    make -C mcu/tools/scripts/build/ MCU_OPTS_FILE=${mcu_opt_file} DSP_OPTS_FILE=${dsp_opt_file}
    exit ${BUILD_RESULT}
fi

