#!/bin/bash
set -v

if [ "$1" == "install" ]; then
#  choco upgrade all -y
  choco install qtcreator -y
fi
