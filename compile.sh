#!/bin/sh

cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "$@" && (cd build && make -j4)
