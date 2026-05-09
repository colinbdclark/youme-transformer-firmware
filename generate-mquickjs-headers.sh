#!/bin/sh
set -e

echo "==> Generating include/mquickjs_atom.h..."
cd lib/mquickjs
make mqjs_stdlib
./mqjs_stdlib -a -m32 > ../../include/mquickjs_atom.h
make clean
cd ../..
echo "    Done."

echo "==> Generating include/embedded_stdlib.h..."
gcc -Wall -O2 -I lib/mquickjs -o embedded_stdlib \
    src/embedded_stdlib.c lib/mquickjs/mquickjs_build.c lib/mquickjs/cutils.c
./embedded_stdlib -m32 > include/embedded_stdlib.h
rm embedded_stdlib
echo "    Done."
