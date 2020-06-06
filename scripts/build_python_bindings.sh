#!/usr/bin/env bash

if [ ! -d python ]; then
  echo -e "\033[31mPlease run this script at the repo root\e[0m"
  exit 1
fi

echo -e "\033[32mInstalling dependencies\e[0m"
sudo apt-get install -y swig libboost-dev

echo -e "\033[32mBuilding project\e[0m"
rm -rf build
mkdir build && cd build
cmake .. -DUSE_EHOME=ON
make -j
cp libices.so ../python

echo -e "\033[32mGenerating wrapper by swig...\e[0m"
swig -DUSE_EHOME \
  -I../../opencv-swig/lib \
  -I/usr/include \
  -I../include \
  -I../src \
  -I../thirdparty/HCEHOMESDK/linux64/include \
  -I../thirdparty/HCPLAYM4SDK/linux64/include \
  -python -c++ ../python/ehome_server.i

echo -e "\033[32mGenerating shared library by g++...\e[0m"
g++ \
  -DUSE_EHOME \
  -shared -fpic ../python/ehome_server_wrap.cxx \
  -I../../opencv-swig/lib \
  -I/usr/include \
  -I../include \
  -I../src \
  -I../thirdparty/HCEHOMESDK/linux64/include \
  -I../thirdparty/HCPLAYM4SDK/linux64/include \
  -L. \
  -L../thirdparty/HCEHOMESDK/linux64/lib \
  -L../thirdparty/HCPLAYM4SDK/linux64/lib \
  -lices \
  -lpthread \
  $(pkg-config --cflags --libs python3) \
  $(pkg-config --libs opencv) \
  --std=c++11 -Wall -O2 \
  -o ../python/_ehome_server.so

echo -e "\033[32mDone!\e[0m"