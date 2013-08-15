@echo off

set DISK=%cd:~0,1%
set BC31=%DISK%:\BC31
set PATH=%BC31%\BIN;%PATH%
REM %BC31%\OS\BIN;

REM -mh Compile using huge memory model
REM -r- Suppresses the use of register variables
REM -Od Disable all optimizations
REM -V Smart C++ virtual tables
REM -Vb Make virtual base class pointer same size as this pointer of the class
REM -nBIN
echo %* -mh -etest.exe -r- -Od -V -Vb -I%BC31%\include;%BC31%\OS\inc;%BC31%\OS\jt\inc;%BC31%\OS;%BC31%\OS\primeri -L%BC31%\lib;%BC31%\OS\lib > cf.txt 