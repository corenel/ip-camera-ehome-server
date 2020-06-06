#!/usr/bin/env bash

if [ ! -d thirdparty ]; then
  echo -e "\033[31mPlease run this script at the repo root\e[0m"
  exit 1
fi

echo -e "\033[32mInstalling third-party libraries\e[0m"
sudo cp -f thirdparty/HCEHOMESDK/linux64/include/* /usr/local/include
sudo cp -f thirdparty/HCEHOMESDK/linux64/lib/* /usr/local/lib
sudo cp -f thirdparty/HCPLAYM4SDK/linux64/include/* /usr/local/include
sudo cp -f thirdparty/HCPLAYM4SDK/linux64/lib/* /usr/local/lib

echo -e "\033[32mDone!\e[0m"
