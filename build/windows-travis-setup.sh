#!/bin/bash
set -v

if [ "$1" == "install" ]; then
  choco install qtcreator -ry
  ls -al c:/tools/qtcreator

fi
