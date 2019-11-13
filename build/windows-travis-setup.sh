#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
  echo "Nothing to do for before_install..."
elif [ "$1" == "install" ]; then
  choco install qt-sdk-windows-x86-mingw_opengl -y
  which qmake
fi
