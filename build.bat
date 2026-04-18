@echo off
set PATH=%PATH%;C:\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin;C:\Program Files\CMake\bin
cmake -S . -B ../RetroIT-mingw -DCMAKE_INSTALL_PREFIX="../Impressive Title" -DCMAKE_BUILD_TYPE=Release
cd ../RetroIT-mingw
mingw32-make install
cmd