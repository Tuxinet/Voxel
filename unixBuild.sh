#!/bin/bash
mkdir -p build
cd build
cmake -S ../ -B . -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -j
make && make Shaders && mangohud ./LveEngine
cd ..
