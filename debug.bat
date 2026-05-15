@echo off
cd .\build\
cmake .. -G"MinGW Makefiles"
mingw32-make
cls
%1