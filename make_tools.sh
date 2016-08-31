#!/bin/bash
set -e
cd ./tools
cd ./topaz
./build_topaz.sh
cd ../scheduler
./build_scheduler.sh
cd ../Profiler
./build_profiler.sh
cd ../..

