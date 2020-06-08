#!/usr/bin/env bash

if [ ! -d python ]; then
  echo -e "\033[31mPlease run this script at the repo root\e[0m"
  exit 1
fi

if [ ! -d build ]; then
  echo -e "\033[31mPlease build the project before building python bindings\e[0m"
  exit 1
fi

echo -e "\033[32mInstalling dependencies\e[0m"
sudo apt-get install -y swig libboost-dev

echo -e "\033[32mGenerating wrapper by swig...\e[0m"
cp build/libices.so python
swig -DUSE_EHOME \
  -I../opencv-swig/lib \
  -I/usr/include \
  -Iinclude \
  -Isrc \
  -Ithirdparty/HCEHOMESDK/linux64/include \
  -Ithirdparty/HCPLAYM4SDK/linux64/include \
  -python -c++ python/ehome_server.i

echo -e "\033[32mGenerating shared library by g++...\e[0m"
g++ \
  -DUSE_EHOME \
  -shared -fpic python/ehome_server_wrap.cxx \
  -I../opencv-swig/lib \
  -I/usr/include \
  -Iinclude \
  -Isrc \
  -Ithirdparty/HCEHOMESDK/linux64/include \
  -Ithirdparty/HCPLAYM4SDK/linux64/include \
  -Lbuild \
  -Lthirdparty/HCEHOMESDK/linux64/lib \
  -Lthirdparty/HCPLAYM4SDK/linux64/lib \
  -lices \
  -lpthread \
  $(pkg-config --cflags --libs python3) \
  $(pkg-config --libs opencv) \
  --std=c++11 -Wall -O2 \
  -o python/_ehome_server.so


echo -e "\033[32mInstalling python package...\e[0m"
python3 -m pip install -e python --user

echo -e "\033[32mDone!\e[0m"