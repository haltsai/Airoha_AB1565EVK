echo off
findstr /v "IotLog_Parse.bat" "RightClickMenu.reg" >"temp.txt"
del /q "RightClickMenu.reg"
ren "temp.txt" "RightClickMenu.reg"
::gen IotLog_Parse.bat
set install_path="%cd%
::set install_path=%install_path:\=\\%
set install_suffix=\IotOfflineSST.exe" %%1 bt
echo %install_path%%install_suffix% >IotLog_Parse.bat
echo pause >>IotLog_Parse.bat

::gen reg file
set install_path=%cd%
set install_prefix=@="
set install_suffix=\\IotLog_Parse.bat \"%%l\""
set install_path=%install_path:\=\\%
echo %install_prefix%%install_path%%install_suffix% >>RightClickMenu.reg
echo on
RightClickMenu.reg
pause
