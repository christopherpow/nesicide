#!/bin/bash
set -v

if [ "$1" == "install" ]; then
  echo "Getting make..."
  choco install make -y
  echo "Getting Qt..."
  choco install qtcreator --version 4.9.0 -y
fi
