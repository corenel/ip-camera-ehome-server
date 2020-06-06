#!/usr/bin/env bash

if [ ! -d src ]; then
  echo -e "\033[31mPlease run this script at the repo root\e[0m"
  exit 1
fi

echo -e "\033[32mBuilding project\e[0m"
rm -rf build
mkdir build && cd build
cmake .. -DUSE_EHOME=ON
make -j

echo -e "\033[32mDone!\e[0m"
