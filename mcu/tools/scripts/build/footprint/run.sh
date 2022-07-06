#!/bin/bash

###############################################################################

footprint_file_name="footprint.csv"

if [ -f "${footprint_file_name}" ]; then
    rm ${footprint_file_name}
fi

echo "Feature Name, Flash, SYSRAM, TCM, FLash, IRAM, DRAM" >  ${footprint_file_name}

./build.sh ab1565_evk earbuds_ref_design -o=MTK_INEAR_ENHANCEMENT=n -o=MTK_DUALMIC_INEAR=n; ./gen_footprint_info.sh "1-MIC_NR"
./build.sh ab1565_evk earbuds_ref_design -o=MTK_INEAR_ENHANCEMENT=n -o=MTK_DUALMIC_INEAR=y; ./gen_footprint_info.sh "2+1_EC_NR"
./build.sh ab1565_evk earbuds_ref_design -o=MTK_INEAR_ENHANCEMENT=y -o=MTK_DUALMIC_INEAR=n; ./gen_footprint_info.sh "1+1_EC_NR"
