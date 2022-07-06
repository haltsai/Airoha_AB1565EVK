#!/bin/bash

###############################################################################
# Variables
feature_name=$1
footprint_files=""
footprint_file_name="footprint.csv"
footprint_content=""
feature_file_name=""
footprint_python_script="mcu/tools/mtk/footprint_gen/footprint_gen.py"
footprint_python_mcu_cfg="mcu/tools/mtk/footprint_gen/footprint_gen_mcu.cfg"
footprint_python_dsp_cfg="mcu/tools/mtk/footprint_gen/footprint_gen_dsp.cfg"
FLASH_DSP0=""
IRAM=""
DRAM=""
debug="1"

if [ -z "${feature_name}" ]; then
    echo "usage:"
    echo "./gen_footprint_info.sh <feature_name>"
    exit 1;
fi

echo "feature_name : ${feature_name}"



# function define
divide() {
    result=$(echo "${1}/1024" | bc -l)
    printf "%.${2}f" "${result}"
}

gen_footprint_files () {

    #  check footprint_python_script exist or not
    if [ ! -f "${PWD}/${footprint_python_script}" ]; then
        echo "File ${PWD}/${footprint_python_script} not exist!"
        return 0
    fi

    #  check footprint_python_mcu_cfg exist or not
    if [ ! -f "${PWD}/${footprint_python_mcu_cfg}" ]; then
        echo "File ${PWD}/${footprint_python_mcu_cfg} not exist!"
        return 0
    fi

    #  check footprint_python_dsp_cfg exist or not
    if [ ! -f "${PWD}/${footprint_python_dsp_cfg}" ]; then
        echo "File ${PWD}/${footprint_python_dsp_cfg} not exist!"
        return 0
    fi

    if [ -d "${PWD}/${1}/out" ]; then
        map_files=$(find "${PWD}/${1}/out" |grep -v bootloader |grep map$)
        echo "Footprint files in ${PWD}/${1}/out"
        echo "$map_files"
        echo ""
        # footprint not found
        if [ "${map_files}" == "" ]; then
            echo "No map files!"
            return 0
        fi

        # read line by line
        while read line;
        do
            where_mapFile_is=$(dirname ${line})
            command=""
            if [ "${1}" == "mcu" ]; then
                command="python3 ${footprint_python_script} ${footprint_python_mcu_cfg} ${line} ${where_mapFile_is}/footprint_mcu.log"
            elif [ "${1}" == "dsp" ]; then
                command="python3 ${footprint_python_script} ${footprint_python_dsp_cfg} ${line} ${where_mapFile_is}/footprint_dsp.log"
            fi
            
            # run python command
            echo "${command}"
            ${command}
            if [ "${debug}" -eq "1" ]; then
                echo -e "\nLINE: '${line}'";
            fi
        done < <(echo "$map_files")

    else
        echo "Foler ${PWD}/${1}/out not exist!"
        echo "Please put gen_footprint_info.sh to <SDK_ROOT>!"
    fi
}

gen_footprint_csv () {
    if [ -d "${PWD}/${1}/out" ]; then
        footprint_files=$(cd "${PWD}/${1}/out"; find . |grep footprint) 
        echo "Footprint files in ${PWD}/${1}/out"
        echo "$footprint_files"
        echo ""
        # footprint not found
        if [ "${footprint_files}" == "" ]; then
            echo "No footprint.log!"
            return 0
        fi

        # read line by line
        while read line; 
        do 
            feature_file_name=$(echo ${line}| awk -F '/' '{print $(NF-1)}')
            footprint_content_usage_row=$(cat "${PWD}/${1}/out/${line}" |grep usage)
            FLASH_DSP0=$(echo $footprint_content_usage_row | awk '{print $2}')
            IRAM=$(echo $footprint_content_usage_row | awk '{print $3}')
            DRAM=$(echo $footprint_content_usage_row | awk '{print $4}')
            if [ "${debug}" -eq "1" ]; then
                echo -e "\nLINE: '${line}'"; 
                echo "feature: ${feature_file_name}"; 
                echo "usage row: ${footprint_content_usage_row}"; 
                echo "FLASH_DSP0 : ${FLASH_DSP0}"; 
                echo "IRAM : ${IRAM}"; 
                echo "DRAM : ${DRAM}"; 
            fi

            #FLASH_DSP0=$(divide ${FLASH_DSP0} 1)
            #IRAM=$(divide ${IRAM} 1)
            #DRAM=$(divide ${DRAM} 1)

            #echo "${1}/out/${line:2},${FLASH_DSP0},${IRAM},${DRAM}" >> $footprint_file_name
            if [ "${1}" == "mcu" ]; then echo -n ",${FLASH_DSP0},${IRAM},${DRAM}" >> $footprint_file_name; fi
            if [ "${1}" == "dsp" ]; then echo ",${FLASH_DSP0},${IRAM},${DRAM}" >> $footprint_file_name; fi
        done < <(echo "$footprint_files")

    else
        echo "Foler ${PWD}/${1}/out not exist!"
        echo "Please put gen_footprint_info.sh to <SDK_ROOT>!"
    fi
}

calculate_delta () {
    origin_footprint=$1
    fo_footprint=$2
    feature_name=$3
    echo -n "$feature_name , " >> "footprint_result.csv"

    for i in {2..7};
    do
        column_origin=$(cat $origin_footprint | cut -d ',' -f $i )
        column_fo=$(cat $fo_footprint | cut -d ',' -f $i )
        echo " \$column_origin : $column_origin"
        echo " \$column_fo : $column_fo"

        # give default value 0
        if [ "$column_origin" == '' ] ; then
            $column_origin='0'
        fi
        if [ "$column_fo" == '' ] ; then
            $column_fo='0'
        fi

        value=$(( ${column_fo}-${column_origin} ))
        value=$(divide ${vlaue} 1)
        if [ "${i}" == "7" ]; then 
            echo "$value" >> "footprint_result.csv";
        else
            echo -n " $value , " >> "footprint_result.csv"
        fi
        #echo -n " $value , " >> "footprint_result.csv"

    done
}

# main process

gen_footprint_files mcu
gen_footprint_files dsp

#echo -n "${feature_name}" >> $footprint_file_name
gen_footprint_csv mcu
gen_footprint_csv dsp

if [ -d "${PWD}/out" ]; then
    mv $footprint_file_name "${PWD}/out"
    mv out ${feature_name}
fi

# out_1-MIC_NR/footprint.csv  and out_origin/footprint.csv exist ,  do copy and compare and redirect to file footprint_result.csv
# use folder name to know which feature option switch
echo "${PWD}/origin/footprint.csv!"
echo "${PWD}/${feature_name}/footprint.csv!"
if [ ${feature_name} != "origin" ] ; then
        calculate_delta "${PWD}/origin/footprint.csv" "${PWD}/${feature_name}/footprint.csv" "${feature_name}"
fi

./build.sh clean
