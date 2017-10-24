#!/bin/bash
set -ev

if [ "$1" == "before_install" ]; then
  brew update
elif [ "$1" == "install" ]; then
  brew install qt5
fi
