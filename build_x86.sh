#!/bin/bash
set -e

mkdir -p build_x86
cmake . -B build_x86
cmake --build build_x86 --verbose
