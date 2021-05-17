@echo off
windres --target=pe-x86-64 resources.rc resources.o
vmake -b -o wavmu.exe -L -mwindows -L -luser32 -L -lcomctl32 -L -ld2d1 -L -lshlwapi -x resources.o clang++ -O2 -std=c++20 -Wall -Wextra