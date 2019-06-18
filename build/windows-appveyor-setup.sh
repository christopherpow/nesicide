#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
  echo "Running vcvars..."
  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
elif [ "$1" == "install" ]; then
  echo "Getting make..."
  choco install make -y
fi
