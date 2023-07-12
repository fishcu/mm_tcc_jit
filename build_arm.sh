#!/bin/bash
set -e

mkdir -p build
cmake . -DBUILD_FOR_MM=ON -B build
cmake --build build --verbose
