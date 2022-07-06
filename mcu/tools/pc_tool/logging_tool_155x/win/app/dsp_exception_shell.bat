@echo off
setlocal ENABLEDELAYEDEXPANSION

@IF EXIST gdb.txt del gdb.txt

::call dsp configuration
call %1

::call *.out
::xt-gdb --command=mtk_gdb_cmds %2
xt-gdb --command=temp/mtk_gdb_cmds %2

::clear temperary files
::for /r .\ %%i in (dump_mem_cmds_2811_*.bin) do del  %%i
::IF EXIST %tarfile% del %tarfile%
::IF EXIST dump_reg_cmds_2811.txt del dump_reg_cmds_2811.txt
::IF EXIST dump_mem_cmds_2811.txt del dump_mem_cmds_2811.txt

::@echo completed call stack reconstruction