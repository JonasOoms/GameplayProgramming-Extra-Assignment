@echo off
pushd ..\
rmdir /S /Q .vs
rmdir /S /Q out
pushd build
del /S /Q *.lib
del /S /Q *.exp
del /S /Q *.ilk
del /S /Q *.pdb
del /S /Q *.ini
popd
PAUSE