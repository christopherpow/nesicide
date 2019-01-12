#!/bin/bash

if [ "$1" == "install" ]; then
  choco upgrade all -y
  choco install qtcreator -y
fi
