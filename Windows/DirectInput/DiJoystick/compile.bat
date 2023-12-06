@echo off

cls
if not defined DevEnvDir (
    REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
    REM call "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
)

set _OUT_=DiJoyStick.exe
set _IN_=DiJoyStick.cpp

REM /W4 
set _CMD_=cl.exe /EHsc /nologo /Fe:%_OUT_% %_IN_%

CALL:ECHORED "[*] %_CMD_%"
%_CMD_%

REM pause

:ECHORED
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Green %1
goto:eof

