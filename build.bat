@echo off

cd ..
IF NOT EXIST bin mkdir bin
pushd bin
echo ========================================================================

REM del /Q * > NUL 2> NUL
cl -nologo -MTd -Od -Gm -Zi -fp:precise ..\src\win32entry.cpp -DDEBUG=1 /Fe:darya /Fo:darya
popd
cd src
