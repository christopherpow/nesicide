#!/bin/bash
set -v

if [ "$1" == "install" ]; then
  choco install qtcreator -rvy
fi
