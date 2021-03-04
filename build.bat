@echo off

cd ..
IF NOT EXIST bin mkdir bin
pushd bin
echo ========================================================================

REM del /Q * > NUL 2> NUL
cl -nologo -MTd -Od -Gm -Zi -fp:precise ..\src\entry.cpp
popd
cd src
