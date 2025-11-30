#!/bin/bash

echo $PATH

rm -rf build
mkdir build
cd build

cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON

cmake --build . -j$(nproc)

QT_QPA_PLATFORM=xcb ./bin/hermeticTPC -i

