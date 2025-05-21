@echo off
pushd ..\
rmdir /S /Q .vs
rmdir /S /Q build
rmdir /S /Q out
popd
PAUSE