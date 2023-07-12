#!/bin/bash
set -e
cmake . -DBUILD_FOR_MM=OFF -B build_x86
cmake --build build_x86
