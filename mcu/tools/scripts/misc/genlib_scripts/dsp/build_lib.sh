#!/bin/bash
CWD=${PWD}
cd ${CWD} && cd middleware/MTK/dspalg/clk_skew_protected&&make clean&&make&&make prebuilt_install&&make clean
