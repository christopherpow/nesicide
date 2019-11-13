#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
  echo "Nothing to do for before_install..."
elif [ "$1" == "install" ]; then
  choco install qtcreator -y
fi
