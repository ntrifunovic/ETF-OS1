@echo off

clos @BUILD

REM set BUILD_LIST=

REM for /f "tokens=*" %%i in (BUILD) do call :concat %%i

REM @echo on
REM @clos %BUILD_LIST%
REM goto :eof

REM :concat
REM set BUILD_LIST=%BUILD_LIST% %1
REM goto :eof