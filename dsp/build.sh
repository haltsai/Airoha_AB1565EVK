#!/bin/bash

###############################################################################
# Variables
export PROJECT_LIST=$(find project | grep "XT-XCC/Makefile$")
export FEATURE_LIST=$(find project | grep "XT-XCC/feature.*.mk$")
export BOARD_LIST="project/*"
export OUT="${PWD}/out"
feature_mk=""
use_new_out_folder="false"
do_make_clean="none"
pwdLength=""
cobuild="off"
pwd_folder_length_limit=23
out_folder_length_limit=65

platform=$(uname)
if [[ "${platform}" =~ "MINGW" || "${platform}" =~ "MSYS" ]]; then
    max_jobs=$(( $(WMIC CPU Get NumberOfLogicalProcessors|tail -2|awk '{print $1}') - 1))
elif [[ "${platform}" =~ "Linux" ]]; then
    max_jobs=`cat /proc/cpuinfo |grep ^processor|wc -l`
    version_id=`cat /etc/os-release | grep VERSION_ID`
    echo "OS_${version_id}"
    if [[ "${version_id}" =~ "19.04" || "${version_id}" =~ "19.10" ]]; then
        echo "XTENSA is not support Ubuntu 19.04 and 19.10!"
        echo "Recommended Ubuntu version 18.10!"
        exit 1;
    fi
fi

# check /bin/bash
if [ -f /bin/bash ]; then
    export SHELL="/bin/bash"
fi

export EXTRA_VAR=-j${max_jobs}

source tools/scripts/build/usage_function

target_check () {
    for p in ${PROJECT_LIST}
    do
        q=$(echo ${p} | grep "project/${1}/")
        if [ ! -z "${q}" ]; then
            r=$(echo ${q} | sed 's/XT-XCC\/Makefile//')
            s=`basename ${r}`
            if [ "$s" == "$2" ]; then
                if [ ${use_new_out_folder} == "false" ]; then
                    OUT=${OUT}/${1}/${2}
                fi
                export TARGET_PATH=$(dirname ${q})
                return 0
            fi
        fi
    done
    return 1
}

clean_out () {
    rm -rf "${1}"
    echo "rm -rf ${1}"
}

check_folder_length () {
    #echo -e "\nout_folder_length_limit=$out_folder_length_limit"
    out_folder_length=$(echo $2| wc -m)
    if [[ "${platform}" =~ "MINGW" || "${platform}" =~ "MSYS" ]]; then
        if [ "$(( out_folder_length - 1 ))" -gt "$(( pwd_folder_length_limit + out_folder_length_limit)) " ]; then
            echo -e "\n Error: over maximum path length"
            if [ "$(( pwdLength - 1 ))" -gt "${pwd_folder_length_limit}" ]; then
                echo -e "    1. Please install SDK in a path less than ${pwd_folder_length_limit} characters"
            fi

            # here's out_folder_length using absolute path
            if [ "$(( out_folder_length - pwdLength ))" -gt "${out_folder_length_limit}" ]; then
                echo -e "    2. Please shorter the name of project or feature."
            fi
            exit 1
        else
            return 0
        fi
    fi

}

###############################################################################
# Begin here
# Parsing arguments
if [ "$#" -eq "0" ]; then
    show_usage
    exit 1
fi

declare -a argv=($0)
ori_argv=$@
for i in $@
do
    case $i in
        -o=*|--option=*)
            opt=" ${i#*=}"
            echo "${opt}" | grep -q -E " OUT="
            if [[ $? -eq 0 ]]; then
                OUT=`echo ${opt} | grep -o "OUT=[^ |^	]*" | cut -d '=' -f2 | tr -d ' '`
                if [ -z "${OUT}" ]; then
                    echo "Error: -o=OUT= cannot be empty!"
                    exit 1
                fi
                OUT=${PWD}/${OUT}
                use_new_out_folder="true"
                echo "output folder change to: ${OUT}"
            fi
            extra_opt+=${opt}
            shift
            ;;
        -f=*|--feature=*)
            feature_mk="${i#*=}"
            shift
            ;;
        list)
            if [ "${#argv[@]}" == "3" ]; then
                target_check ${argv[1]} ${argv[2]}
                if [ "$?" -eq "0" ]; then
                    show_available_proj ${argv[1]} ${argv[2]}
                    exit 0
                else
                    echo "Error: ${argv[1]} ${argv[2]} is not available board & project"
                    show_usage
                    exit 1
                fi
            elif [ "${#argv[@]}" == "1" ]; then
                show_available_proj
                exit 0
            else
                #echo "args : ${#argv[@]}"
                echo "Error: wrong usage!!"
                exit 1
            fi
            ;;
        -cobuild)
            cobuild="on"
            ;;
        bl)
            echo "ignore the parameter: \"${i}\""
            ;;
        -*)
            echo "Error: unknown parameter \"${i}\""
            show_usage
            exit 1
            ;;
        *)
            argv+=(${i})
            ;;
    esac
done

# To do actions according to arguments
if [ "${argv[3]}" == "clean" ]; then
    if [ "${#argv[@]}" != "4" ]; then
        show_usage
        exit 1
    fi
    if [ "${use_new_out_folder}" == "true" ]; then
        clean_out ${OUT}
    else
        clean_out ${OUT}/${argv[1]}/${argv[2]}
    fi
elif [ "${argv[2]}" == "clean" ]; then
    if [ "${#argv[@]}" != "3" ]; then
        show_usage
        exit 1
    fi
    if [ "${use_new_out_folder}" == "true" ]; then
        clean_out ${OUT}
    else
        clean_out ${OUT}/${argv[1]}
    fi
elif [ "${argv[1]}" == "clean" ]; then
    if [ "${#argv[@]}" != "2" ]; then
        show_usage
        exit 1
    fi
    clean_out ${OUT}
else
    if [ "${#argv[@]}" != "3" ]; then
        show_usage
        exit 1
    fi
    target_check ${argv[1]} ${argv[2]}
    if [ "$?" -ne "0" ]; then
        echo "Error: ${argv[1]} ${argv[2]} is not available board & project"
        show_usage
        exit 1
    fi

    feature_mk_path=${TARGET_PATH}
    if [ ! -z ${feature_mk} ]; then
        if [ ! -e "${TARGET_PATH}/${feature_mk}" ]; then
            mkdir -p ${OUT}/log
            echo "Error: cannot find ${feature_mk} under ${TARGET_PATH}."
            echo "Error: cannot find ${feature_mk} under ${TARGET_PATH}." > ${OUT}/log/err.log
            echo "FAIL" > ${OUT}/log/build_time.log
            exit 1
        fi
        EXTRA_VAR+=" FEATURE=${feature_mk}"
    else
        feature_mk_path=`grep "^TARGET_PATH\ *[?:]\{0,1\}=\ *" ${TARGET_PATH}/Makefile | cut -d '=' -f2 | tr -d ' ' | tail -1`
        if [ -z ${feature_mk}_path ]; then
            feature_mk_path=$TARGET_PATH
        fi
        feature_mk=`grep "^FEATURE\ *[?:]\{0,1\}=\ *" ${TARGET_PATH}/Makefile | cut -d '=' -f2 | tr -d ' ' | tail -1`
        echo "FEATURE = ${feature_mk}"
    fi
    if [ -e "${OUT}/$TARGET_PATH/tmp.mk" ]; then
        diff -q ${feature_mk_path}/${feature_mk} ${OUT}/${TARGET_PATH}/tmp.mk
        if [ $? -ne 0 ]; then
            do_make_clean="true"
        fi
    fi
    if [ -e "${OUT}/${TARGET_PATH}/extra_opts.lis" ]; then
        echo ${extra_opt} | grep -Eo  "[_[:alnum:]]+=[[:graph:]]*" | sort | uniq > ${OUT}/${TARGET_PATH}/extra_opts.current
        diff -q ${OUT}/${TARGET_PATH}/extra_opts.current ${OUT}/${TARGET_PATH}/extra_opts.lis
        if [ $? -ne 0 ]; then
            do_make_clean="true"
        else
            if [ ${do_make_clean} != "true" ]; then
                do_make_clean="false"
                rm -f "${OUT}/${TARGET_PATH}/extra_opts.current"
            fi
        fi
    fi
    if [ ${do_make_clean} == "true" ]; then
        clean_out ${OUT}
    fi
    mkdir -p "${OUT}/${TARGET_PATH}"
    echo ${extra_opt} | grep -Eo  "[_[:alnum:]]+=[[:graph:]]*" | sort | uniq > ${OUT}/${TARGET_PATH}/extra_opts.lis
    cp -f "${feature_mk_path}/${feature_mk}" "${OUT}/${TARGET_PATH}/tmp.mk"
    rm -rf "${OUT}/log"
    mkdir -p "${OUT}/log"
    echo "$0 ${ori_argv}" > ${OUT}/log/build_time.log
    echo "Start Build: "`date` >> ${OUT}/log/build_time.log
    EXTRA_VAR+="${extra_opt}"
    echo "make -C ${TARGET_PATH} OUTDIR=${OUT} ${EXTRA_VAR} 2>> ${OUT}/log/err.log"
    make -C ${TARGET_PATH} OUTDIR=${OUT} ${EXTRA_VAR} 2>> ${OUT}/log/err.log
    BUILD_RESULT=$?
    echo "End Build: "`date` >> ${OUT}/log/build_time.log
    cat ${OUT}/log/build.log | grep "MODULE BUILD" >> ${OUT}/log/build_time.log
    if [ "${BUILD_RESULT}" -eq "0" ]; then
        echo "DSP warnings: "`grep warning: -c ${OUT}/log/err.log ` >> ${OUT}/log/build_time.log
        echo "DSP build log: ${OUT}/log/err.log" >> ${OUT}/log/build_time.log
        echo "TOTAL BUILD: PASS" >> ${OUT}/log/build_time.log
    else
        echo "TOTAL BUILD: FAIL" >> ${OUT}/log/build_time.log
    fi
    echo "=============================================================="
    cat "${OUT}/log/build_time.log"
    mv -f ${OUT}/*.log "${OUT}/log/" 2> /dev/null
    exit ${BUILD_RESULT}

fi

