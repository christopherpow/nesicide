#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
elif [ "$1" == "install" ]; then
  echo "Getting make..."
  choco install make -y
fi
