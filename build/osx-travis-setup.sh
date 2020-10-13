#!/bin/bash

if [ "$1" == "before_install" ]; then
  brew update
elif [ "$1" == "install" ]; then
  brew install qt@5.12.6
  brew link qt5 --force
fi
