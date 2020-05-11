#!/bin/bash
basepath=$(cd `dirname $0`; pwd)

dir=$basepath/build_release
mkdir -p "$dir"
cd "$dir"
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
